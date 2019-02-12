#ifndef UTILS_H
#define UTILS_H

#include <ESP8266WiFi.h>

namespace Utils
{
	static void setupLed()
	{
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, HIGH);
	}

	static void blinkLed(void)
	{
		digitalWrite(LED_BUILTIN, LOW);
		delay(50);
		digitalWrite(LED_BUILTIN, HIGH);
	}

	// you need to fill in your data
	static const String SSID;   // cannot be longer than 32 characters!
	static const String SSID_PASS;
	static const String MQTT_TOPIC;
	static const IPAddress MQTT_SERVER;
};

#endif
