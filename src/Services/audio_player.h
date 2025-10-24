#pragma once
#include <Arduino.h>
#include <FS.h>
#include "MP3DecoderHelix.h"
#include "AACDecoderHelix.h"
#include "driver/i2s.h"
#include "LittleFS.h"
#include "user_config.h"

class AudioPlayer
{
public:
    AudioPlayer(i2s_port_t port = I2S_PORT);
    void begin(BaseType_t core = 0);
    void playFile(const char *path);
    void stop();
    void setOnPlayDoneCallback(std::function<void(const char* file)> cb)
    {
        _onPlayDone = cb;
    }

private:
    std::function<void(const char* file)> _onPlayDone;
    // Worker task
    static void _taskEntry(void *param);
    void _taskLoop();

    // Decoder runners
    void _aacRun(Stream *input);
    void _mp3Run(Stream *input);

    // Helpers
    void _i2sInit(uint32_t sample_rate);
    const char *_getFileExt(const char *filename);

    // Data callbacks
    static void _mp3Callback(MP3FrameInfo &info, int16_t *samples, size_t len, void *ref);
    static void _aacCallback(AACFrameInfo &info, int16_t *samples, size_t len, void *ref);

private:
    i2s_port_t _i2s_num;
    uint32_t _last_samplerate = 0;
    QueueHandle_t _queue;
    TaskHandle_t _taskHandle;

    libhelix::MP3DecoderHelix _mp3;
    libhelix::AACDecoderHelix _aac;

    uint8_t *_frame_buf;

    // Command types
    enum CmdType
    {
        CMD_NONE,
        CMD_PLAY,
        CMD_STOP
    };

    enum AudioExt
    {
        AUDIO_MP3,
        AUDIO_ACC,
        AUDIO_NOT_SUPPORTED
    } ;

    struct Msg
    {
        CmdType cmd;
        char filePath[128];
        AudioExt ext;
    };
};
