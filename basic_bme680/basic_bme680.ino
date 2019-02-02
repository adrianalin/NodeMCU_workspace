#include "bsec.h"

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;
char sendBuffer[200];

// Entry point for the example
void setup(void)
{
  Serial.begin(9600);

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

// Function that is looped forever
void loop(void)
{
  if (iaqSensor.run()) // If new data is available
  {
    snprintf(sendBuffer, sizeof(sendBuffer),
                        "{\"IAQ_Accuracy\": \"%d\",\"IAQ\":\"%.2f\""
                        ",\"Temperature\": \"%.2f\",\"Humidity\": \"%.2f\",\"Pressure\": \"%.2f\""
                        ",\"Gas\": \"%.0f\""
                        ",\"Status\": \"%d\"}",
                        iaqSensor.iaqAccuracy, iaqSensor.iaqEstimate,
                        iaqSensor.temperature, iaqSensor.humidity, iaqSensor.pressure / 100,
                        iaqSensor.gasResistance,
                        iaqSensor.status);
    strcat(sendBuffer, "\r\n");
    Serial.println(sendBuffer);
    memset(sendBuffer, 0, sizeof(sendBuffer));
  }
  else
  {
    checkIaqSensorStatus();
  }
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "1 BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
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
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
