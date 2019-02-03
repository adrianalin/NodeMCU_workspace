#ifndef UTILS_H
#define UTILS_H

#include <ESP8266WiFi.h>

namespace Utils
{
	static void setupLed()
	{
		pinMode(LED_BUILTIN, OUTPUT);
	}

	static void blinkLed(void)
	{
		digitalWrite(LED_BUILTIN, LOW);
		delay(50);
		digitalWrite(LED_BUILTIN, HIGH);
	}

	// you need to fill in your data

};

#endif