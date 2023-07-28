#ifndef PurpleHatModule_h
#define PurpleHatModule_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "IoTT_TrainSensor.h"

#define hatSDA 26 // changes between input and output by I2C master
#define hatSCL 0  // pull SCL low while sending non I2C data over SDA

class PurpleHatModule
{
private:
    static IoTT_TrainSensor* _trainSensor;
  

public:
    static void setup();
    static void begin();
    static void loop();

    static void ResetDistance();
    static void ClearHeading();
    static void RepRate(int repRate);
    static void GetSensorData(String& sensorData);
};

#endif