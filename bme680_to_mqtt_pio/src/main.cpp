#include "bme680.h"
#include "mqttclient.h"
#include "utils.h"

BME680 bme680_dev;
MQTTClient mqttclient;

void setup()
{
	Serial.begin(115200);
	delay(10);

	Utils::setupLed();
	mqttclient.setup();
	bme680_dev.setup();
}

void loop()
{
	const char* message = bme680_dev.loop();
	if (strlen(message) > 0)
	{
		mqttclient.loop(message);
	}
}
