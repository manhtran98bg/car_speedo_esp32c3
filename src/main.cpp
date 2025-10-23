#include <Arduino.h>
#include "Drivers/screen_driver.h"
#include "Views/main_view.h"
#include "Views/gif_view.h"
#include "LittleFS.h"

static long lastCmd = 0;

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
	main_view_init();
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

