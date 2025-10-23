#include <Arduino.h>
#include "Drivers/screen_driver.h"
#include "Views/main_view.h"
#include "Views/gif_view.h"
#include "LittleFS.h"
#include "Services/mjpeg_player.h"
static long lastCmd = 0;
static const char *splash_video_file = "/video/splash_2.mjpeg";

static int displayBack(JPEGDRAW *pDraw);

MjpegPlayer *videoPlayer = new MjpegPlayer(displayBack, false, 0, 0, TFT_HOR_RES, TFT_VER_RES);

static int displayBack(JPEGDRAW *pDraw)
{
	int x1 = pDraw->x;
	int y1 = pDraw->y;
	int x2 = x1 + pDraw->iWidth - 1;
	int y2 = y1 + pDraw->iHeight - 1;
	Screen.drawRegion(pDraw->pPixels, x1, y1, x2, y2);
	return 1;
}

static void onVideoPlayDone(const char *file)
{
	if (strcasecmp(file, splash_video_file) == 0)
	{
		main_view_init();
	}
}

void setup() {
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
	videoPlayer->begin(0);
	videoPlayer->setOnPlayDoneCallback(onVideoPlayDone);
	videoPlayer->playFile(splash_video_file);
}

void loop() {
  // put your main code here, to run repeatedly:

  	if (millis() - lastCmd > 30000)
	{
		lastCmd = millis();
		size_t idx = (size_t)(esp_random() % 8 + 1);
		char path[32];
		sprintf(path, "/gif/%d.gif", idx);
		gif_request_show(path);
	}
}

