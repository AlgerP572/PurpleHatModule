#include <SPIFFS.h>

#include "ConfigLoader.h"
#include "WifiDebug.h"

bool ConfigLoader::readFile(const String& fileName,  String& buffer)
{
    if (!SPIFFS.exists(fileName))
    {       
        WifiDebug::print("File" + fileName + "not found\n");
        return false;
    }

    File dataFile = SPIFFS.open(fileName, "r");
    if (!dataFile)
    {        
        WifiDebug::print("Can't open" + fileName +" \n");
        return false;
    }

    buffer.reserve(dataFile.size() + 1);
    while (dataFile.available())
    {
        buffer += dataFile.readString();
    }

    dataFile.close();
    return true;
}

DynamicJsonDocument* ConfigLoader::getDocPtr(const String& cmdFile)
{
    String jsonData;
    
    readFile(cmdFile, jsonData);    
    if (jsonData.length() <= 0)
    {
        WifiDebug::println("File read error");
        return NULL;
    }

    uint16_t docSize = 4096 * (trunc((3 * jsonData.length()) / 4096) + 1);
    WifiDebug::print("Size:" + jsonData +  "Doc Size:" + docSize + "\n");
    DynamicJsonDocument * thisDoc = new DynamicJsonDocument(docSize);
    DeserializationError error;
    error = deserializeJson(*thisDoc, jsonData); 
        
    if (error)
    {
        WifiDebug::println("Deserialization error");
        return NULL;
    } 

    return thisDoc; 
}