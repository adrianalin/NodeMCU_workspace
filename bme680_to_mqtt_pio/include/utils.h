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

	static bool loopWiFi()
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			// We start by connecting to a WiFi network
			Serial.println();
			Serial.print("Connecting to ");
			Serial.println(Utils::SSID);

			WiFi.mode(WIFI_STA);
			WiFi.begin(Utils::SSID.c_str(), Utils::SSID_PASS.c_str());
			delay(5000);
			Serial.println("IP address: ");
			Serial.println(WiFi.localIP());
			randomSeed(micros());
			return false;
		}
		return true;
	}
	// you need to fill in your data
	static const String SSID;   // cannot be longer than 32 characters!
	static const String SSID_PASS;
	static const String MQTT_TOPIC;
	static const IPAddress MQTT_SERVER;
};

#endif
