
#include "WifiDebug.h"

// access via the /webserial route.
bool WifiDebug::_serialPortLog = true;
std::map<String, SerialDebugCommands>  WifiDebug::_debugCommands;

void WifiDebug::recvDebugDataMsg(uint8_t *data, size_t len)
{
    WebSerial.println("Received Data...");
    String d = "";
    for(int i=0; i < len; i++){
        d += char(data[i]);
    }

    switch(_debugCommands[d])
    {
        case cmdSerialOn:
           _serialPortLog = true;
        break;
        case cmdSerialOff:
           _serialPortLog = false;
        break;
        default:
        break;
    }
    WebSerial.println(d);
}

void  WifiDebug::InitializeCommands()
{
    _debugCommands["Serial:ON"] = cmdSerialOn;
    _debugCommands["Serial:OFF"] = cmdSerialOff;   
}

void WifiDebug::begin(AsyncWebServer* server)
{
    WebSerial.begin(server);
    WebSerial.msgCallback(recvDebugDataMsg);

    InitializeCommands(); 
}

// Print
void WifiDebug::print(String& m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void  WifiDebug::print(const char *m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void WifiDebug::print(char *m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void WifiDebug::print(int m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void  WifiDebug::print(uint8_t m)
{
    WebSerial.print(m);    
    printSerialPort(m);
}

void  WifiDebug::print(uint16_t m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void  WifiDebug::print(uint32_t m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void  WifiDebug::print(double m)
{
    WebSerial.print(m);
    printSerialPort(m);
}

void  WifiDebug::print(float m)
{
    WebSerial.print(m);
    printSerialPort(m);
}


// Print with New Line

void  WifiDebug::println(String& m)
{
    WebSerial.println(m); 
    printlnSerialPort(m);   
}

void  WifiDebug::println(const char *m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(char *m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(int m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(uint8_t m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(uint16_t m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(uint32_t m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(float m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void  WifiDebug::println(double m)
{
    WebSerial.println(m);
    printlnSerialPort(m);
}

void WifiDebug::printSerialPort(String& s)
{
    if(_serialPortLog == false)
        return;

    Serial.print(s);
}

void WifiDebug::printSerialPort(const char *m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(char *m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(int m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(uint8_t m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(uint16_t m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(uint32_t m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(double m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printSerialPort(float m)
{
    if(_serialPortLog == false)
        return;

    Serial.print(m);
}

void WifiDebug::printlnSerialPort(String& s)
{
    if(_serialPortLog == false)
        return;

    Serial.println(s);
}

void WifiDebug::printlnSerialPort(const char *m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(char *m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(int m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(uint8_t m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(uint16_t m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(uint32_t m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(float m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

void WifiDebug::printlnSerialPort(double m)
{
    if(_serialPortLog == false)
        return;

    Serial.println(m);
}

    
    

