#include <Wire.h>

#include "PurpleHatModule.h"
#include "WifiSerialDebug.h"
#include "ConfigLoader.h"

IoTT_TrainSensor* PurpleHatModule::_trainSensor = NULL;
movingAvg PurpleHatModule::_avgDistance(100);
movingAvg PurpleHatModule::_avgDistanceNoOutliers(100);
movingAvg PurpleHatModule::_speedData(100);
movingAvg PurpleHatModule::_scaleSpeedData(200);
movingAvg PurpleHatModule::_scaleAccelData(200);

LowPass<2> PurpleHatModule::_speedFilter(3, 1e2,true); 
LowPass<2> PurpleHatModule::_accelFilter(3, 1e2,true); 

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

    _avgDistance.begin();
    _avgDistanceNoOutliers.begin();
    _speedData.begin();
    _scaleSpeedData.begin();
    _scaleAccelData.begin();
    
    DynamicJsonDocument* jsonDataObj = ConfigLoader::getDocPtr(String("/configdata/phcfg.cfg"));
    if (jsonDataObj != NULL)
    {
        Serial.println("Load Trainside Sensor");
         _trainSensor = new IoTT_TrainSensor(&Wire,
            &_avgDistance,
            &_avgDistanceNoOutliers,
            &_speedData,
            &_scaleSpeedData,
            &_scaleAccelData,
            &_speedFilter,
            &_accelFilter,
            hatSDA,
            hatSCL);
//        trainSensor->setTxCallback(sendMsg);
         _trainSensor->loadLNCfgJSON(*jsonDataObj);
        delete(jsonDataObj); 
         Serial.println("Purple Sensor loaded"); 
    }
    else
    {
        Serial.println("/configdata/phcfg.cfg not Found."); 
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

void PurpleHatModule::SetDccAddr(int16_t dccAddr)
{
    if (_trainSensor != NULL)
    {

        // Curerntly only suppoting WiThrottle.
        _trainSensor->reqDCCAddrWatch(NULL,
            dccAddr,
             true); //17: WiThrottle Client                
    }
}

void PurpleHatModule::GetSensorData(String& sensorData)
{
    if (_trainSensor != NULL)
    {
        _trainSensor->processSensorLoop(sensorData);
    }
    else
    {
        sensorData.clear();
    }    
}

void PurpleHatModule::GetSpeedTestData(String& speedTableData)
{
    if (_trainSensor != NULL)
    {
        _trainSensor->processSpeedTestLoop(speedTableData);
    }
}

void PurpleHatModule::startTest(float_t trackLen, float_t vMax, uint8_t pMode)
{    
    if (_trainSensor != NULL)
    {
        Log::print("Start test: ", LogLevel::INFO);
        Log::print("trackLen = ", LogLevel::INFO);
        Log::println(trackLen, LogLevel::INFO);

        _trainSensor->startTest(trackLen, vMax, pMode);
    }
}

void PurpleHatModule::stopTest(String& speedTableData)
{
     if (_trainSensor != NULL)
     {
        _trainSensor->stopTest(speedTableData);
     }
}