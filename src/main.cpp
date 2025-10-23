#include <Arduino.h>
#include "Drivers/screen_driver.h"
#include "Views/main_view.h"

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

  Screen.begin();
  main_view_init();
}

void loop() {
  // put your main code here, to run repeatedly:
}

