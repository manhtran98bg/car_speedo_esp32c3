#include "audio_player.h"

#define MP3_FRAME_SIZE (MP3_MAX_FRAME_SIZE)
#define AUDIO_STACK_SIZE 8192
#define AUDIO_QUEUE_LEN 4

AudioPlayer::AudioPlayer(i2s_port_t port)
    : _i2s_num(port), _queue(nullptr), _taskHandle(nullptr)
{
    _frame_buf = (uint8_t *)malloc(MP3_FRAME_SIZE);
}

void AudioPlayer::begin(BaseType_t core)
{
    _i2sInit(SAMPLE_RATE);
    i2s_zero_dma_buffer(_i2s_num);

    _mp3.setDataCallback(_mp3Callback);
    _aac.setDataCallback(_aacCallback);
    _mp3.setReference(this);
    _aac.setReference(this);
    _queue = xQueueCreate(AUDIO_QUEUE_LEN, sizeof(Msg));
    xTaskCreatePinnedToCore(_taskEntry, "AudioTask", AUDIO_STACK_SIZE, this,
                            configMAX_PRIORITIES - 3, &_taskHandle, core);

    Serial.println("🎧 AudioPlayer initialized");
}

void AudioPlayer::playFile(const char *path)
{
    if (!_queue)
        return;
    Msg msg = {CMD_PLAY, "", AUDIO_NOT_SUPPORTED};
    strncpy(msg.filePath, path, sizeof(msg.filePath) - 1);
    const char *ext = _getFileExt(path);
    if (strcasecmp(ext, "aac") == 0)
    {
        msg.ext = AUDIO_ACC;
    }
    else if (strcasecmp(ext, "mp3") == 0)
    {
        msg.ext = AUDIO_MP3;
    }
    xQueueSend(_queue, &msg, portMAX_DELAY);
}

void AudioPlayer::stop()
{
    if (!_queue)
        return;
    Msg msg = {CMD_STOP};
    xQueueSend(_queue, &msg, portMAX_DELAY);
}

void AudioPlayer::_taskEntry(void *param)
{
    auto *self = static_cast<AudioPlayer *>(param);
    self->_taskLoop();
    vTaskDelete(nullptr);
}

void AudioPlayer::_taskLoop()
{
    Msg msg;
    Stream *input = nullptr;
    AudioExt ext = AUDIO_NOT_SUPPORTED;
    int r, w;
    for (;;)
    {
        if (xQueueReceive(_queue, &msg, portMAX_DELAY))
        {
            switch (msg.cmd)
            {
            case CMD_PLAY:
            {
                File *f = new File(LittleFS.open(msg.filePath));
                if (!f)
                    return;
                ext = msg.ext;
                Serial.println("🎵 Start playback");

                if (ext == AUDIO_ACC)
                    _aacRun(f);
                else if (ext == AUDIO_MP3)
                    _mp3Run(f);
                else
                    Serial.printf("⚠️ Unsupported extension: %s\n", ext);
                if (_onPlayDone)
                    _onPlayDone(msg.filePath);
                break;
            }

            case CMD_STOP:
                Serial.println("⏹ Stop requested");
                _aac.end();
                _mp3.end();
                break;

            default:
                break;
            }
        }
    }
}

void AudioPlayer::_aacRun(Stream *input)
{
    int r, w;
    _aac.begin();
    while (input && input->available())
    {
        r = input->readBytes((char *)_frame_buf, MP3_FRAME_SIZE);
        if (r <= 0)
            break;

        int remaining = r;
        while (remaining > 0)
        {
            w = _aac.write(_frame_buf + (r - remaining), remaining);
            remaining -= w;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    Serial.println("AAC playback done");
    _aac.end();
}

void AudioPlayer::_mp3Run(Stream *input)
{
    int r, w;
    _mp3.begin();
    while (input && input->available())
    {
        r = input->readBytes((char *)_frame_buf, MP3_FRAME_SIZE);
        if (r <= 0)
            break;

        int remaining = r;
        while (remaining > 0)
        {
            w = _mp3.write(_frame_buf + (r - remaining), remaining);
            remaining -= w;
        }
        vTaskDelay(1);
    }
    Serial.println("MP3 playback done");
    _mp3.end();
    if (input)
        input->flush();
}

void AudioPlayer::_i2sInit(uint32_t sample_rate)
{
    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = sample_rate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 256,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0,
        .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
        .bits_per_chan = I2S_BITS_PER_CHAN_16BIT};

    i2s_pin_config_t pins = {
        .mck_io_num = -1,
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_DIN,
        .data_in_num = -1};

    esp_err_t err = i2s_driver_install(_i2s_num, &cfg, 0, NULL);
    err |= i2s_set_pin(_i2s_num, &pins);
    if (err == ESP_OK)
        Serial.println("✅ I2S initialized");
    else
        Serial.println("❌ I2S init failed");
}

const char *AudioPlayer::_getFileExt(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

void AudioPlayer::_mp3Callback(MP3FrameInfo &info, int16_t *samples, size_t len, void *ref)
{
    static uint32_t last_rate = 0;
    if (last_rate != info.samprate)
    {
        i2s_set_clk(I2S_PORT, info.samprate, info.bitsPerSample,
                    (info.nChans == 2) ? I2S_CHANNEL_STEREO : I2S_CHANNEL_MONO);
        last_rate = info.samprate;
    }

    size_t written = 0;
    i2s_write(I2S_PORT, samples, len * 2, &written, portMAX_DELAY);
}

void AudioPlayer::_aacCallback(AACFrameInfo &info, int16_t *samples, size_t len, void *ref)
{
    static uint32_t last_rate = 0;
    if (last_rate != info.sampRateOut)
    {
        i2s_set_clk(I2S_PORT, info.sampRateOut, info.bitsPerSample,
                    (info.nChans == 2) ? I2S_CHANNEL_STEREO : I2S_CHANNEL_MONO);
        last_rate = info.sampRateOut;
    }

    size_t written = 0;
    i2s_write(I2S_PORT, samples, len * 2, &written, portMAX_DELAY);
}
