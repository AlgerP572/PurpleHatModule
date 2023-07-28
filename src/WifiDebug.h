#ifndef WifiDebug_h
#define WifiDebug_h

#include <Arduino.h>
#include <WebSerial.h>
#include <map>

typedef  enum serialDebugCommands
{
    cmdSerialOn,
    cmdSerialOff
} SerialDebugCommands;

class WifiDebug
{

private:    

    // Web Serial already defines this.
    // static WebSerial _webSerial; 
    static bool _serialPortLog;
    static std::map<String, SerialDebugCommands> _debugCommands;

    static void recvDebugDataMsg(uint8_t *data, size_t len);
    static void printSerialPort(String& s);
    static void printSerialPort(const char *m);
    static void printSerialPort(char *m);
    static void printSerialPort(int m);
    static void printSerialPort(uint8_t m);
    static void printSerialPort(uint16_t m);
    static void printSerialPort(uint32_t m);
    static void printSerialPort(double m);
    static void printSerialPort(float m);

    static void printlnSerialPort(String& s);
    static void printlnSerialPort(const char *m);
    static void printlnSerialPort(char *m);
    static void printlnSerialPort(int m);
    static void printlnSerialPort(uint8_t m);
    static void printlnSerialPort(uint16_t m);
    static void printlnSerialPort(uint32_t m);
    static void printlnSerialPort(float m);
    static void printlnSerialPort(double m);


    static void InitializeCommands();

public:    
    static void begin(AsyncWebServer* server);
    static void print(String& m);
    static void print(const char *m);
    static void print(char *m);
    static void print(int m);
    static void print(uint8_t m);
    static void print(uint16_t m);
    static void print(uint32_t m);
    static void print(double m);
    static void print(float m);

    // Print with New Line
    static void println(String& m);
    static void println(const char *m);
    static void println(char *m);
    static void println(int m);
    static void println(uint8_t m);
    static void println(uint16_t m);
    static void println(uint32_t m);
    static void println(float m);
    static void println(double m);
};

#endif