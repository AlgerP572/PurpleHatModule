#include <Wire.h>

#include "PurpleHatModule.h"
#include "WifiDebug.h"
#include "ConfigLoader.h"

IoTT_TrainSensor* PurpleHatModule::_trainSensor = NULL;

void PurpleHatModule::setup()
{
    Serial.println("Init TwoWire");
    Wire.begin(hatSDA, hatSCL);//, 400000); //initialize the I2C interface 400kHz
    delay(10);
//        Serial.println(Wire.getClock());
//        Serial.println("Set Clock");  
    Wire.setClock(400000);
    delay(10);
//        Serial.println(Wire.getClock());

    DynamicJsonDocument* jsonDataObj = ConfigLoader::getDocPtr("/configdata/phcfg.cfg", false);
    if (jsonDataObj != NULL)
    {
         Serial.println("Load Trainside Sensor");
         _trainSensor = new IoTT_TrainSensor(&Wire, hatSDA, hatSCL);
//        trainSensor->setTxCallback(sendMsg);
         _trainSensor->loadLNCfgJSON(*jsonDataObj);
         delete(jsonDataObj); 
         Serial.println(F("Purple Sensor loaded")); 
    }
    else
    {
         Serial.println(F("/configdata/phcfg.cfg not Found.")); 
    }
}

void PurpleHatModule::begin()
{
}

void PurpleHatModule::loop()
{
}

void PurpleHatModule::ResetDistance()
{
     if (_trainSensor != NULL)
        _trainSensor->resetDistance();
}

void PurpleHatModule::ClearHeading()
{
     if (_trainSensor != NULL)
        _trainSensor->resetHeading();
}

void PurpleHatModule::RepRate(int repRate)
{
     if (_trainSensor != NULL)
        _trainSensor->setRepRate(NULL, repRate);
}

String PurpleHatModule::GetSensorData()
{
    if (_trainSensor != NULL)
    {
        return _trainSensor->processLoop();
    }
    else
    {
        return {};
    }
}