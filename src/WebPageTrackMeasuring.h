#ifndef WebPageTrackMeasuring_h
#define WebPageTrackMeasuring_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

class WebPageTrackMeasuring
{

private: 
    static char _html[];
    static AsyncWebSocket _ws;
    static AsyncEventSource _events;
    static int _millisRollOver;
    static unsigned long _lastMillis;
    static String BBVersion;

    // Timer variables
    static unsigned long _lastTime;  
    static unsigned long _timerDelay;    
    
    static void notifyClients();
    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                 AwsEventType type, void *arg, uint8_t *data, size_t len);
    static String processor(const String& var);
    static void GetStats(String& jsonData);    

public:
    static void begin(AsyncWebServer* server);
    static void loop();
};

#endif