#include <SPIFFS.h>

#include "ConfigLoader.h"
#include "WifiDebug.h"

char ConfigLoader::_wsTxBuffer[BufferSize];

uint32_t ConfigLoader::readFileToBuffer(String fileName, char* thisBuffer, uint32_t maxSize)
{
    uint32_t bytesRead = 0;
    //  Serial.printf("Trying to read File %s\n", &fileName[0]);
    if (SPIFFS.exists(fileName))
    {
        File dataFile = SPIFFS.open(fileName, "r");
        if (dataFile)
        {
        if (dataFile.size() < maxSize)
        {
            bytesRead = dataFile.size();
            dataFile.read((uint8_t*)thisBuffer, bytesRead);
            thisBuffer[bytesRead] = '\0'; 
        }
        dataFile.close();
        }
        else 
        Serial.printf("Can't open %s \n", &fileName[0]);
    } 
    else 
        Serial.printf("File %s not found\n", &fileName[0]);
    return bytesRead;
}

DynamicJsonDocument* ConfigLoader::getDocPtr(String cmdFile, bool duplData)
{
    uint32_t jsonData = readFileToBuffer(cmdFile,
        _wsTxBuffer,
        BufferSize);
    Serial.println(_wsTxBuffer);
    if (jsonData > 0)
    {
        uint16_t docSize = 4096 * (trunc((3 * jsonData) / 4096) + 1);  //.length();
        Serial.printf("Size: %i Doc Size: %i\n", jsonData, docSize);
        DynamicJsonDocument * thisDoc = new DynamicJsonDocument(docSize);
        DeserializationError error;
        if (duplData)
            error = deserializeJson(*thisDoc, (const char*) _wsTxBuffer); //use const to force deserialize to keep copy of buffer data
        else
            error = deserializeJson(*thisDoc, _wsTxBuffer); //use const to force deserialize to keep copy of buffer data
        if (!error)
        {
            return thisDoc;
        }
        else
        {
            Serial.println("Deserialization error");
            return NULL;
        }
    }
    else
    {
        Serial.println("File read error");
         return NULL;
    }
}