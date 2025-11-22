#include <Arduino.h>

#include "Views/main_view.h"
#include "Views/gif_view.h"
#include "LittleFS.h"
#include "Services/mjpeg_player.h"
#include "Services/audio_player.h"

#include "Drivers/Display/ScreenDriver.h"
#include "Drivers/Display/CanvasLvgl.h"
#include "Drivers/Display/CanvasLGFX.h"
#include "Drivers/Display/CanvasManagerLvgl.h"
#include "Drivers/Display/CanvasManagerLGFX.h"
#include "Face.h"

static long lastCmd = 0;
static const char *splash_video_file = "/video/splash.mjpeg";
static const char *splash_audio_file = "/audio/splash.aac";
static const char *welcome_audio_file = "/audio/welcome.aac";
static int displayBack(JPEGDRAW *pDraw);

MjpegPlayer *videoPlayer;
AudioPlayer *audioPlayer;

ICanvasManager *canvasManager = new CanvasManagerLGFX();
Face *face;
ICanvas *canvas;

static const char *nav[] = {
	"/gif/go_ahead.gif",
	"/gif/go_left.gif",
	"/gif/go_right.gif",
	"/gif/turn_left.gif",
	"/gif/turn_right.gif"};
static int displayBack(JPEGDRAW *pDraw)
{
	int x1 = pDraw->x;
	int y1 = pDraw->y;
	int x2 = x1 + pDraw->iWidth - 1;
	int y2 = y1 + pDraw->iHeight - 1;
	Screen.drawRegion(pDraw->pPixels, x1, y1, x2, y2);
	return 1;
}

static void onGifPlayDone(const char *file)
{
	size_t idx = (size_t)(esp_random() % 5);
	gif_request_show(nav[idx]);
}
static void onVideoPlayDone(const char *file)
{
	if (strcasecmp(file, splash_video_file) == 0)
	{
		main_view_init();
		// gif_onPlayDoneCallback(onGifPlayDone);
		// gif_request_show("/gif/1.gif");
	}
}

static void onAudioPlayDone(const char *file)
{
	if (strcasecmp(file, splash_audio_file) == 0)
	{
		// audioPlayer->playFile(welcome_audio_file);
	}
}

void setup()
{
	// put your setup code here, to run once:
	delay(2000);
	Serial.begin(115200);

	// Version Arduino Core
	Serial.print("Arduino Core Version: ");
	Serial.printf("%d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);

	// ESP-IDF Version
	Serial.print("ESP-IDF Version: ");
	Serial.println(esp_get_idf_version());
	if (!LittleFS.begin(true))
	{
		Serial.println("LittleFS Mount Failed");
		return;
	}
	Screen.begin();
	int id = canvasManager->createCanvas(240, 240, 1);
	if (id == -1)
	{
		Serial.println("Create canvas failed");
		return;
	}
	canvas = canvasManager->getCanvasWrapper(id);
	if (canvas)
	{
		face = new Face(canvas, 50, 240, 240, BLACK, YELLOW);
	}
	// main_view_init();
	// gif_onPlayDoneCallback(onGifPlayDone);
	// gif_request_show("/gif/go_left.gif");
	// videoPlayer = new MjpegPlayer(displayBack, false, 0, 0, TFT_HOR_RES, TFT_VER_RES);
	// audioPlayer = new AudioPlayer();
	// videoPlayer->begin(0);
	// audioPlayer->begin(0);
	// videoPlayer->setOnPlayDoneCallback(onVideoPlayDone);
	// audioPlayer->setOnPlayDoneCallback(onAudioPlayDone);
	// videoPlayer->playFile(splash_video_file);
	// audioPlayer->playFile(splash_audio_file);
}

void loop()
{
	static size_t lastCheckHeap = millis();
	// put your main code here, to run repeatedly:

	// if (millis() - lastCmd > 30000)
	// {
	// 	lastCmd = millis();
	// 	size_t idx = (size_t)(esp_random() % 8 + 1);
	// 	char path[32];
	// 	sprintf(path, "/gif/%d.gif", idx);
	// 	gif_request_show(path);
	// }
	if (millis() - lastCheckHeap > 1000)
	{
		Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
		lastCheckHeap = millis();
	}
	face->Update();
}
