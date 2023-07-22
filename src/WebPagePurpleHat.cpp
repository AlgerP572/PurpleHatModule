#include "WebPagePurpleHat.h"
#include "WifiDebug.h"

char WebPagePurpleHat::_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Purple Hat Sensor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" type="text/css" href="style.css">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <style>
    .topnav { background-color: #800080; }   
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>Purple Hat Sensor</h1>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> TEMPERATURE</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-tint" style="color:#00add6;"></i> HUMIDITY</p><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> PRESSURE</p><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
      </div>
      <div class="card">
          <p class="card-title">Temperature Chart</p>
          <div id="chart-temperature" class="chart-container"></div>
        </div>
    </div>
  </div>
  <script src="script.js"></script>
</body>
</html>)rawliteral";

AsyncWebSocket WebPagePurpleHat::_ws("/ws");
AsyncEventSource WebPagePurpleHat::_events("/events");
bool WebPagePurpleHat::_ledState = false;
unsigned long  WebPagePurpleHat::_lastTime = 0;  
unsigned long WebPagePurpleHat::_timerDelay = 500;
float WebPagePurpleHat::_temperature = 0;
float WebPagePurpleHat::_humidity = 0;
float WebPagePurpleHat::_pressure = 0;
JSONVar WebPagePurpleHat:: _readings;

void WebPagePurpleHat::notifyClients()
{
  _ws.textAll(String(_ledState));
}

void WebPagePurpleHat::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      _ledState = !_ledState;
      notifyClients();
    }
  }
}

void WebPagePurpleHat::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    char msg[50];

    switch (type)
    {
    case WS_EVT_CONNECT:
        sprintf(msg,
            "WebSocket client #%u connected from %s\n",
            client->id(),
            client->remoteIP().toString().c_str());
        Serial.printf(msg);
        WifiDebug::print(msg);
        break;
    case WS_EVT_DISCONNECT:
        sprintf(msg,
            "WebSocket client #%u disconnected\n",
            client->id());
        Serial.printf(msg);
        WifiDebug::print(msg);
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void WebPagePurpleHat::begin(AsyncWebServer* server)
{
  _ws.onEvent(onEvent);
  server->addHandler(&_ws);

  // Handle Web Server Events
  _events.onConnect([](AsyncEventSourceClient *client)
  {
    if(client->lastId())
    {
        char msg[50];

        sprintf(msg,
         "Client reconnected! Last message ID that it got is: %u\n",
          client->lastId());
        Serial.printf(msg);
        WifiDebug::print(msg);
    }

    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 1000);
  });
  server->addHandler(&_events);

  // Route for root / web page
  server->on("/purplehat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200,
     "text/html",
      _html,
       processor);
  });

  server->on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getSensorReadingsJSON();
    request->send(200, "application/json", json);
    json = String();
  });
}

String  WebPagePurpleHat::processor(const String& var){
  getSensorReadings();
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(_temperature);
  }
  else if(var == "HUMIDITY"){
    return String(_humidity);
  }
  else if(var == "PRESSURE"){
    return String(_pressure);
  }
  return String();
}

void WebPagePurpleHat::loop()
{
    if ((millis() - _lastTime) > _timerDelay)
    {
        getSensorReadings();
        char msg[50];
        sprintf(msg,
            "Temperature = %.2f ºC \n",
            _temperature);
        WifiDebug::print(msg);
        
        sprintf(msg,
            "Humidity = %.2f \n",
             _humidity);
        WifiDebug::print(msg);
        
        sprintf(msg,
            "Pressure = %.2f hPa \n",
             _pressure);
        WifiDebug::print(msg);
        WifiDebug::println("-----");

        // Send Events to the Web Client with the Sensor Readings
        _events.send("ping",NULL,millis());
        _events.send(String(_temperature).c_str(),"temperature",millis());
        _events.send(String(_humidity).c_str(),"humidity",millis());
        _events.send(String(_pressure).c_str(),"pressure",millis());

        // send JSON object
         _events.send(getSensorReadingsJSON().c_str(),"new_readings" ,millis());
        _lastTime = millis();
    }
}

void WebPagePurpleHat::getSensorReadings()
{
    _temperature += 1.0; 
    _humidity += 2;
    _pressure += 3;
}

String WebPagePurpleHat::getSensorReadingsJSON()
{  
  _readings["sensor1"] = String(_temperature);
  _readings["sensor2"] = String(_humidity);
  _readings["sensor3"] = String(_pressure);
  _readings["sensor4"] = String(0);

  String jsonString = JSON.stringify(_readings);
  return jsonString;
}