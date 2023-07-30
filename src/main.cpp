#include <Arduino.h>
#include <M5Unified.h>
#include <SPIFFS.h>

#include "WebServer.h"
#include "WebPageTrackMeasuring.h"
#include "WebPageCV.h"
#include "WebPagePurpleHat.h"
#include "WifiConnection.h"
#include "WifiSerialDebug.h"
#include "WifiFirmware.h"
#include "NTPTimeClient.h"

#include "PurpleHatModule.h"

unsigned long _lastTime;  
unsigned long _timerDelay = 10000;

void setup()
{
    M5.begin(); 
    M5.Lcd.setTextSize(3);
    M5.Lcd.setRotation(3);  
    M5.Lcd.println("Purple Hat");

    WiFi.mode(WIFI_STA); 
    Serial.begin(115200);

    AsyncWebServer* server = WebServer::get();
    DNSServer* dns = WebServer::dns();

    bool result = WifiConnection::setup(server, dns, false);

    if(result == false) 
    {
     
        Serial.println("Failed to connect");
        M5.Lcd.println("Failed to connect");
        ESP.restart();
    }
  
    // If you get here you have connected to the WiFi    
    Serial.println("connected...");
    M5.Lcd.println("Engine: ");    
    M5.Lcd.println(WiFi.localIP());
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), IPAddress(8,8,8,8)); 

    // Initialize a NTPClient to get time
    TimeClient::begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    TimeClient::setTimeOffset(3600);

    if(!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    server->serveStatic("/", SPIFFS, "/");


    // Note: Before this point OTA logging does not work.
    // The next line starts the servcies required for OTA
    // logging. Anything using OTA logging including web
    // pages must be after this line.
    Log::begin(server);
  
    // Start supported services
    WebPageTrackMeasuring::begin(server);
    WebPageCv::begin(server);
    WebPagePurpleHat::begin(server);
    WifiFirmware::begin(server);
    WebServer::begin();
  

    // Start underlying hardware modules
    PurpleHatModule::setup(); 
         
    M5.Lcd.println("HTTP server started");    
}

void loop()
{ 
    u32_t time = millis();

    if(!TimeClient::update())
    {
        TimeClient::forceUpdate();
    }
    
    if ((time - _lastTime) > _timerDelay)
    {
        Log::print("Time: ", LogLevel::WATCHDOG);
        Log::println(time, LogLevel::WATCHDOG);
        _lastTime = time;
        
        // The formattedDate comes with the following format:
        // 2018-05-28T16:00:13Z
        // We need to extract date and time
        String formattedTime;
        TimeClient::getFormattedDate(formattedTime);
        Log::println(formattedTime, LogLevel::WATCHDOG);
    }

    delay(2);
    WebPageTrackMeasuring::loop();
    delay(2);
    WebPagePurpleHat::loop();  

    // This will "feed the watchdog".
    delay(2);
    return;  
}
