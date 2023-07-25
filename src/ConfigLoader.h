#ifndef ConfigLoader_h
#define ConfigLoader_h

#include <ArduinoJson.h>

#define BufferSize 16384

class ConfigLoader
{ 
private:    
    static char _wsTxBuffer[BufferSize];
   

public:    
    static DynamicJsonDocument* getDocPtr(String cmdFile, bool duplData);
     static uint32_t readFileToBuffer(String fileName, char * thisBuffer, uint32_t maxSize); 
};

#endif