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
    static bool _ledState;

    static float _temperature;
    static float _humidity;
    static float _pressure;

    // Timer variables
    static unsigned long _lastTime;  
    static unsigned long _timerDelay;    
    
    static void notifyClients();
    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                 AwsEventType type, void *arg, uint8_t *data, size_t len);
    static String processor(const String& var);
    static void getSensorReadings();
    static String getSensorReadingsJSON();

public:
    static void begin(AsyncWebServer* server);
    static void loop();
};

#endif