#include "mjpeg_player.h"

#define READ_BUFFER_SIZE 1024
#define MJPEG_BUFFER_SIZE (240 * 60)

MjpegPlayer::MjpegPlayer(JPEG_DRAW_CALLBACK *pfnDraw,
                         bool useBigEndian,
                         int x, int y,
                         int width,
                         int height)
{
    _pfnDraw = pfnDraw;
    _useBigEndian = useBigEndian;
    _widthLimit = width;
    _heightLimit = height;
    _inputindex = 0;
    _x = x;
    _y = y;
    _queue = xQueueCreate(3, sizeof(mjpeg_msg_t));
    _mjpeg_buf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);
    _read_buf = (uint8_t *)malloc(READ_BUFFER_SIZE);

    if (!_mjpeg_buf || !_read_buf)
    {
        Serial.println("❌ MjpegPlayer malloc failed!");
        vTaskDelete(nullptr);
    }
}

void MjpegPlayer::begin(BaseType_t core)
{
    xTaskCreatePinnedToCore(
        _taskEntry,
        "MjpegPlayerTask",
        8096, // 8KB stack
        this,
        configMAX_PRIORITIES - 3,
        &_taskHandle,
        core);
}

void MjpegPlayer::playFile(const char *path)
{
    mjpeg_msg_t msg = {MJPEG_CMD_PLAY, path};
    xQueueSend(_queue, &msg, portMAX_DELAY);
}

void MjpegPlayer::stop()
{
    mjpeg_msg_t msg = {MJPEG_CMD_STOP, ""};
    xQueueSend(_queue, &msg, portMAX_DELAY);
}

void MjpegPlayer::_taskEntry(void *param)
{
    auto *self = static_cast<MjpegPlayer *>(param);
    if (self)
        self->_taskLoop();
    vTaskDelete(nullptr);
}

void MjpegPlayer::_taskLoop()
{
    mjpeg_msg_t msg;
    static char buf[64];
    while (true)
    {
        if (xQueueReceive(_queue, &msg, portMAX_DELAY))
        {
            if (msg.cmd == MJPEG_CMD_PLAY)
            {
                File file = LittleFS.open(msg.filePath);
                if (!file)
                {
                    Serial.printf("❌ Cannot open %s\n", msg.filePath.c_str());
                    continue;
                }
                Serial.printf("[MJPEG] ▶️ Playing: %s\n", msg.filePath.c_str());
                memset(buf, 0, sizeof(buf));
                strncpy(buf, msg.filePath.c_str(), sizeof(buf) - 1);
                buf[sizeof(buf) - 1] = '\0';
                _stopRequested = false;
                _input = &file;
                _inputindex = 0;
                while (_input->available() && !_stopRequested)
                {
                    if (_readMjpegBuf())
                        _drawJpg();
                    vTaskDelay(pdMS_TO_TICKS(5));
                }

                file.close();
                if (_onPlayDone)
                    _onPlayDone(buf);
                Serial.println("[MJPEG] ⏹️ Playback done");
            }
            else if (msg.cmd == MJPEG_CMD_STOP)
            {
                _stopRequested = true;
                Serial.println("[MJPEG] ⏸️ Stop requested");
            }
        }
    }
}

bool MjpegPlayer::_readMjpegBuf()
{
    if (_inputindex == 0)
    {
        _buf_read = _input->readBytes(_read_buf, READ_BUFFER_SIZE);
        _inputindex += _buf_read;
    }
    _mjpeg_buf_offset = 0;
    int i = 0;
    bool found_FFD8 = false;
    while ((_buf_read > 0) && (!found_FFD8))
    {
        i = 0;
        while ((i < _buf_read) && (!found_FFD8))
        {
            if ((_read_buf[i] == 0xFF) && (_read_buf[i + 1] == 0xD8)) // JPEG header
            {
                // Serial.printf("Found FFD8 at: %d.\n", i);
                found_FFD8 = true;
            }
            ++i;
        }
        if (found_FFD8)
        {
            --i;
        }
        else
        {
            _buf_read = _input->readBytes(_read_buf, READ_BUFFER_SIZE);
        }
    }
    uint8_t *_p = _read_buf + i;
    _buf_read -= i;
    bool found_FFD9 = false;
    if (_buf_read > 0)
    {
        i = 3;
        while ((_buf_read > 0) && (!found_FFD9))
        {
            if ((_mjpeg_buf_offset > 0) && (_mjpeg_buf[_mjpeg_buf_offset - 1] == 0xFF) && (_p[0] == 0xD9)) // JPEG trailer
            {
                // Serial.printf("Found FFD9 at: %d.\n", i);
                found_FFD9 = true;
            }
            else
            {
                while ((i < _buf_read) && (!found_FFD9))
                {
                    if ((_p[i] == 0xFF) && (_p[i + 1] == 0xD9)) // JPEG trailer
                    {
                        found_FFD9 = true;
                        ++i;
                    }
                    ++i;
                }
            }

            // Serial.printf("i: %d\n", i);
            memcpy(_mjpeg_buf + _mjpeg_buf_offset, _p, i);
            _mjpeg_buf_offset += i;
            size_t o = _buf_read - i;
            if (o > 0)
            {
                // Serial.printf("o: %d\n", o);
                memcpy(_read_buf, _p + i, o);
                _buf_read = _input->readBytes(_read_buf + o, READ_BUFFER_SIZE - o);
                _p = _read_buf;
                _inputindex += _buf_read;
                _buf_read += o;
                // Serial.printf("_buf_read: %d\n", _buf_read);
            }
            else
            {
                _buf_read = _input->readBytes(_read_buf, READ_BUFFER_SIZE);
                _p = _read_buf;
                _inputindex += _buf_read;
            }
            i = 0;
        }
        if (found_FFD9)
        {
            return true;
        }
    }

    return false;
}

bool MjpegPlayer::_drawJpg()
{
    _remain = _mjpeg_buf_offset;
    _jpeg.openRAM(_mjpeg_buf, _remain, _pfnDraw);
    if (_scale == -1)
    {
        // scale to fit height
        int iMaxMCUs;
        int w = _jpeg.getWidth();
        int h = _jpeg.getHeight();
        float ratio = (float)h / _heightLimit;
        if (ratio <= 1)
        {
            _scale = 0;
            iMaxMCUs = _widthLimit / 16;
        }
        else if (ratio <= 2)
        {
            _scale = JPEG_SCALE_HALF;
            iMaxMCUs = _widthLimit / 8;
            w /= 2;
            h /= 2;
        }
        else if (ratio <= 4)
        {
            _scale = JPEG_SCALE_QUARTER;
            iMaxMCUs = _widthLimit / 4;
            w /= 4;
            h /= 4;
        }
        else
        {
            _scale = JPEG_SCALE_EIGHTH;
            iMaxMCUs = _widthLimit / 2;
            w /= 8;
            h /= 8;
        }
        _jpeg.setMaxOutputSize(iMaxMCUs);
        _x = (w > _widthLimit) ? 0 : ((_widthLimit - w) / 2);
        _y = (_heightLimit - h) / 2;
    }
    if (_useBigEndian)
    {
        _jpeg.setPixelType(RGB565_BIG_ENDIAN);
    }
    _jpeg.decode(_x, _y, _scale);
    _jpeg.close();

    return true;
}
