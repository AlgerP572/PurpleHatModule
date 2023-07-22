#ifndef WebPageCv_h
#define WebPageCv_h

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

class WebPageCv
{

private: 
    static char _html[];
    static AsyncWebSocket _ws;
    static bool _ledState;
    
    static void notifyClients();
    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                 AwsEventType type, void *arg, uint8_t *data, size_t len);
    static String processor(const String& var);

public:
    static void begin(AsyncWebServer* server);
    static void loop();
};

#endif