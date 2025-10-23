#ifndef _MJPEG_PLAYER_H_
#define _MJPEG_PLAYER_H_

#include "Arduino.h"
#include "LittleFS.h"
#include <JPEGDEC.h>

// ==== Command type ====
typedef enum
{
    MJPEG_CMD_NONE = 0,
    MJPEG_CMD_PLAY,
    MJPEG_CMD_STOP
} mjpeg_cmd_t;

// ==== Command struct ====
typedef struct
{
    mjpeg_cmd_t cmd;
    String filePath;
} mjpeg_msg_t;

class MjpegPlayer
{
public:
    MjpegPlayer(JPEG_DRAW_CALLBACK *pfnDraw,
                bool useBigEndian,
                int x, int y,
                int width,
                int height);

    void begin(BaseType_t core = 1);
    void playFile(const char *path);
    void stop();
    void setOnPlayDoneCallback(std::function<void(const char* file)> cb)
    {
        _onPlayDone = cb;
    }


private:
    std::function<void(const char* file)> _onPlayDone;
    static void _taskEntry(void *param);
    void _taskLoop();

    bool _readMjpegBuf();
    bool _drawJpg();

private:
    // FreeRTOS task & queue
    QueueHandle_t _queue;
    TaskHandle_t _taskHandle;
    JPEG_DRAW_CALLBACK *_pfnDraw;

    // Config
    bool _useBigEndian;
    bool _stopRequested = false;

    // Decode buffer context
    Stream *_input;
    uint8_t *_mjpeg_buf;
    uint8_t *_read_buf;
    int32_t _inputindex = 0;
    int32_t _buf_read = 0;
    int32_t _mjpeg_buf_offset = 0;
    int _x;
    int _y;
    int _widthLimit;
    int _heightLimit;
    JPEGDEC _jpeg;
    int _scale = -1;
    int32_t _remain = 0;
};

#endif // _MJPEG_PLAYER_H_
