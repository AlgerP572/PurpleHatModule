
#include "WifiDebug.h"

// access via the /webserial route.
bool WifiDebug::_serialPortLog = false;
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
void WifiDebug::print(String m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void  WifiDebug::print(const char *m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void WifiDebug::print(char *m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void WifiDebug::print(int m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void  WifiDebug::print(uint8_t m)
{
    WebSerial.print(m);    
    printSerialPort(String(m));
}

void  WifiDebug::print(uint16_t m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void  WifiDebug::print(uint32_t m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void  WifiDebug::print(double m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}

void  WifiDebug::print(float m)
{
    WebSerial.print(m);
    printSerialPort(String(m));
}


// Print with New Line

void  WifiDebug::println(String m)
{
    WebSerial.println(m); 
    printlnSerialPort(String(m));   
}

void  WifiDebug::println(const char *m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(char *m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(int m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(uint8_t m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(uint16_t m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(uint32_t m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(float m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void  WifiDebug::println(double m)
{
    WebSerial.println(m);
    printlnSerialPort(String(m));
}

void WifiDebug::printSerialPort(String s)
{
    if(_serialPortLog == false)
        return;

    Serial.print(s);
}

void WifiDebug::printlnSerialPort(String s)
{
    if(_serialPortLog == false)
        return;

    Serial.println(s);
}

    
    

