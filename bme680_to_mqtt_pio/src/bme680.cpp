#include "bme680.h"
#include "utils.h"

void BME680::setup()
{
	m_iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
	m_output = "\nBSEC library version " + String(m_iaqSensor.version.major)
			+ "." + String(m_iaqSensor.version.minor)
			+ "." + String(m_iaqSensor.version.major_bugfix)
			+ "." + String(m_iaqSensor.version.minor_bugfix);
	Serial.println(m_output);
	checkIaqSensorStatus();

	bsec_virtual_sensor_t sensorList[10] = {
		BSEC_OUTPUT_RAW_TEMPERATURE,
		BSEC_OUTPUT_RAW_PRESSURE,
		BSEC_OUTPUT_RAW_HUMIDITY,
		BSEC_OUTPUT_RAW_GAS,
		BSEC_OUTPUT_IAQ,
		BSEC_OUTPUT_STATIC_IAQ,
		BSEC_OUTPUT_CO2_EQUIVALENT,
		BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
		BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
		BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY
	};

	m_iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
	checkIaqSensorStatus();
}

const char* BME680::loop()
{
	memset(m_sendBuffer, 0, sizeof(m_sendBuffer));
	if (m_iaqSensor.run()) // new data available
	{
		snprintf(m_sendBuffer, sizeof(m_sendBuffer),
			"{\"IAQ_Accuracy\": \"%d\",\"IAQ\":\"%.1f\""
			",\"Temperature\": \"%.1f\",\"Humidity\": \"%.1f\",\"Pressure\": \"%.0f\""
			",\"CO2\": \"%.0f\""
			",\"Status\": \"%d\"}",
			m_iaqSensor.iaqAccuracy, m_iaqSensor.iaqEstimate,
			m_iaqSensor.temperature, m_iaqSensor.humidity, m_iaqSensor.pressure / 100,
			m_iaqSensor.co2Equivalent,
			m_iaqSensor.status);
		strcat(m_sendBuffer, "\r\n");
		Serial.print(String("bme680 - ") + String(m_sendBuffer));
	}
	else
	{
		checkIaqSensorStatus();
	}
	return m_sendBuffer;
}

void BME680::checkIaqSensorStatus(void)
{
	if (m_iaqSensor.status != BSEC_OK)
	{
		if (m_iaqSensor.status < BSEC_OK)
		{
			m_output = "BSEC error code : " + String(m_iaqSensor.status);
			Serial.println(m_output);
			for (;;)
				Utils::blinkLed(); /* Halt in case of failure */
		}
		else
		{
			m_output = "BSEC warning code : " + String(m_iaqSensor.status);
			Serial.println(m_output);
		}
	}

	if (m_iaqSensor.bme680Status != BME680_OK)
	{
		if (m_iaqSensor.bme680Status < BME680_OK)
		{
			m_output = "BME680 error code : " + String(m_iaqSensor.bme680Status);
			Serial.println(m_output);
			for (;;)
				Utils::blinkLed(); /* Halt in case of failure */
		}
		else
		{
			m_output = "BME680 warning code : " + String(m_iaqSensor.bme680Status);
			Serial.println(m_output);
		}
	}
}
