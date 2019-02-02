#include "bsec.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid =  "";   // cannot be longer than 32 characters!
const char *pass =  "";
const String TOPIC = "indoor/bme680";
IPAddress server(192, 168, 0, 206);
WiFiClient wclient;
PubSubClient client(wclient);
long lastMsg = 0;


void checkIaqSensorStatus(void);
void blinkLed(void);
Bsec iaqSensor;
String output;
char sendBuffer[200];

void setup_bme680()
{
  iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[8] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 8, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "1 BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        blinkLed(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "2 BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        blinkLed(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void blinkLed(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC.c_str(), "hello world");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // Setup console
  Serial.begin(9600);
  delay(10);
  setup_wifi();
  client.setServer(server, 1883);

  setup_bme680();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis() / 1000;
  if (now - lastMsg > 3500)
  {
    lastMsg = now;

    if (iaqSensor.run()) // If new data is available
    {
      snprintf(sendBuffer, sizeof(sendBuffer),
                          "{\"IAQ_Accuracy\": \"%d\",\"IAQ\":\"%.1f\""
                          ",\"Temperature\": \"%.1f\",\"Humidity\": \"%.1f\",\"Pressure\": \"%.0f\""
                          ",\"Gas\": \"%.0f\""
                          ",\"Status\": \"%d\"}",
                          iaqSensor.iaqAccuracy, iaqSensor.iaqEstimate,
                          iaqSensor.temperature, iaqSensor.humidity, iaqSensor.pressure / 100,
                          iaqSensor.gasResistance,
                          iaqSensor.status);
      strcat(sendBuffer, "\r\n");

      int msgLen = strlen(sendBuffer);
      client.beginPublish(TOPIC.c_str(), msgLen, false);
      for (int i = 0; i < msgLen; i++)
      {
        client.print(sendBuffer[i]);
      }
      client.endPublish();
      blinkLed();
      memset(sendBuffer, 0, sizeof(sendBuffer));
    }
    else
    {
      checkIaqSensorStatus();
    }
  }
}
