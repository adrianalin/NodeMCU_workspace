#include "mqttclient.h"
#include "utils.h"

void MQTTClient::setup()
{
	m_MQTTclient.setClient(m_espclient);
	m_MQTTclient.setServer(Utils::MQTT_SERVER, 1883);
}

void MQTTClient::loop(const char* sendBuffer)
{
	if (!m_MQTTclient.connected())
	{
		reconnect();
		return;
	}
	m_MQTTclient.loop();

	long now = millis() / 1000;
	if (now - m_lastMsg > 3600)
	{
		m_lastMsg = now;
		int msgLen = strlen(sendBuffer);

		m_MQTTclient.beginPublish(Utils::MQTT_TOPIC.c_str(), msgLen, false);
		for (int i = 0; i < msgLen; i++)
		{
			m_MQTTclient.print(sendBuffer[i]);
		}
		m_MQTTclient.endPublish();
		Serial.println(String("mqtt - ") + String(sendBuffer));
		Utils::blinkLed();
	}
}

void MQTTClient::reconnect()
{
	Serial.print("Attempting MQTT connection...");
	// Create a random client ID
	String clientId = "ESP8266Client-";
	clientId += String(random(0xffff), HEX);
	// Attempt to connect
	if (m_MQTTclient.connect(clientId.c_str()))
	{
		Serial.println("connected");
	}
	else
	{
		Serial.print("failed, rc=");
		Serial.print(m_MQTTclient.state());
		Serial.println(" try again in 5 seconds");
		// Wait 5 seconds before retrying
		delay(5000);
	}
}
