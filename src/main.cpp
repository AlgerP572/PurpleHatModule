#include <Arduino.h>
#include <M5StickCPlus.h>
#include <SPIFFS.h>

#include "WebServer.h"
#include "WebPageRoot.h"
#include "WebPageCV.h"
#include "WebPagePurpleHat.h"
#include "WifiConnection.h"
#include "WifiDebug.h"
#include "WifiFirmware.h"

void setup()
{
  M5.begin(); 
  M5.Lcd.setTextSize(3);
  M5.Lcd.setRotation(3);  
  M5.Lcd.println("DCC over WiFi");

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

  if(!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  server->serveStatic("/", SPIFFS, "/");  
  
  // Start supported services
  WebPageRoot::begin(server);
  WebPageCv::begin(server);
  WebPagePurpleHat::begin(server);
  WifiFirmware::begin(server);
  WifiDebug::begin(server);
  
  WebServer::begin();    
  M5.Lcd.println("HTTP server started");    
}

void loop()
{ 
  u32_t time = millis();
  WifiDebug::print("Time: ");
  WifiDebug::print(time);
  WifiDebug::println(" Hello!");
  WebPagePurpleHat::loop();
  delay(10);
  return;  
}
