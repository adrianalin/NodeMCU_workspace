#include "bme680.h"
#include "mqttclient.h"
#include "utils.h"

BME680 multisensor;
MQTTClient mqttclient;

void setup()
{
  Serial.begin(9600);
  delay(10);

  Utils::setupLed();
  mqttclient.setup();
  multisensor.setup();
}

void loop()
{
  const char* message = multisensor.loop();
  mqttclient.loop(message);
  delay(5000);
}
