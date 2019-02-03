#ifndef BME680_H
#define BME680_H

#include <bsec.h>

class BME680
{
public:

  void checkIaqSensorStatus(void);
  void setup();
  const char* loop();

private:
  Bsec m_iaqSensor;
  String m_output;
  char m_sendBuffer[200];
};

#endif