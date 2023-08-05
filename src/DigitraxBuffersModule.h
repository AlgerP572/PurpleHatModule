#ifndef DigitraxBuffersModule_h
#define DigitraxBuffersModule_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "IoTT_DigitraxBuffers.h"

class DigitraxBuffersModule
{
private:
    static IoTT_DigitraxBuffers* _digitraxBuffer;
  

public:
    static void setup(txFct lnOut);
    static void begin();
    static void loop();
};

#endif