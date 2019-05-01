#include "bme680.h"
#include "mqttclient.h"
#include "utils.h"
#include "ota.h"

BME680 bme680_dev;
MQTTClient mqttclient;
OTAUpdate update;

void setup()
{
	Serial.begin(115200);
	delay(10);

	Utils::loopWiFi();
	Utils::setupLed();
	mqttclient.setup();
	bme680_dev.setup();
	update.setup();
}

void loop()
{
	const char* message = bme680_dev.loop();
	if (strlen(message) > 0)
	{
		mqttclient.loop(message);
	}
	Utils::loopWiFi();
	update.loop();
}
