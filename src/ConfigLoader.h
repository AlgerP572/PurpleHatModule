#ifndef ConfigLoader_h
#define ConfigLoader_h

#include <ArduinoJson.h>

class ConfigLoader
{ 
public:    
    static DynamicJsonDocument* getDocPtr(const String& cmdFile);
    static bool readFile(const String& fileName,  String& buffer);
};

#endif