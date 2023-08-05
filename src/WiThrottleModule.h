#ifndef WiThrottleModule_h
#define WiThrottleModule_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "IoTT_lbServer.h"

class WiThrottleModule
{
private:
    static IoTT_LBServer* _lbClient;
  

public:
    static void setup();
    static void begin();
    static void loop();

    static uint16_t sendMsg(lnTransmitMsg txData);
};

#endif