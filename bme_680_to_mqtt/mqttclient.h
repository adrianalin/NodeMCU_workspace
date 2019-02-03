#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

class MQTTClient
{
public:
	void reconnect();
	void setup();
	void loop(const char* sendBuffer);

private:
	WiFiClient m_espclient;
	PubSubClient m_MQTTclient;
	long m_lastMsg = 0;
};

#endif