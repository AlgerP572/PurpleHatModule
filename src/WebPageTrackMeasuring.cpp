#include "WebPageTrackMeasuring.h"
#include "WifiDebug.h"

#include "PurpleHatModule.h"
#include "ConfigLoader.h"

char WebPageTrackMeasuring::_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Purple Hat Sensor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" type="text/css" href="style.css">
  <script
      src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.9.1/chart.min.js"
      integrity="sha512-ElRFoEQdI5Ht6kZvyzXhYG9NqjtkmlkfYk0wr6wHxU9JEHakS7UJZNeml5ALk+8IKlU6jDgMabC3vkumRokgJA=="
      crossorigin="anonymous"
      referrerpolicy="no-referrer"
    ></script>
  <style>
    .topnav { background-color: #800080; }   
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>Track Measuring Display</h1>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <div class="button-container">
          <button id="btnStart" class="button" onClick="startMeasuring(this)">Start</button>
          <button id="btnStart" class="button" onClick=" resetDistance(this)">Reset Distance</button>
        </div>                   
        <p><table class="datatable">
        <tr>
          <th>Data</th>
          <th>Value</th>
          <th>Units</th>          
        </tr>
         <tr>
          <td>Scale</td>
          <td><span id="scale">%Scale%</span></td>
          <td><span id="scaleunit">%Scaleunit%</span></td>        
        </tr>
        <tr>
          <td>Wheel Diameter</td>
          <td><span id="wheeldiameter">%Wheel Diameter%</span></td>
          <td>[mm]</td>          
        </tr>
         <tr>
          <td>Angle</td>
          <td><span id="angle">%angle%</span></td>
          <td>[&deg]</td>          
        </tr>
         <tr>
          <td>Reverse Direction</td>
          <td><span id="reversedirection">%reversedirection%</span></td>
          <td></td>          
        </tr>
        <tr>
          <td>Direction</td>
          <td><span id="direction">%Direction%</span></td>
          <td></td>          
        </tr>
        <tr>
          <td>Measured Speed</td>
          <td><span id="measuredspeed">%Measured Soeed%</span></td>
          <td>[mm/s]</td>          
        </tr>        
        <tr>
          <td>Scale Speed</td>
          <td><span id="scalespeed">%scalespeed%</span></td>
          <td>[km/h]</td>
        </tr>
        <tr>
          <td>Absolute Distance</td>
          <td><span id="absdistance">%absdistance%</span></td>
          <td>[cm]</td>  
        </tr>
        <tr>
          <td>Relative Distance</td>
          <td><span id="reldistance">%reldistance%</span></td>
          <td>[cm]</td>  
        </tr>
      </table></p>
      </div>      
      <div class="card">
          <p class="card-title">Speed Data</p>
          <canvas id="chart-speed-data" width="600" height="400"></canvas>
        </div>
    </div>
  </div>
  <div class="topnav">
    <p><table class="footertable">
      <tr>
        <td>Date / Time</th>
        <td><span id="datetime">%datetime%</span></th>
        <td>IP Address</th>
        <td><span id="ipaddress">%datetime%</span></th>
        <td>Firmware Version</th>
        <td><span id="fwversion">%datetime%</span></th>    
      </tr>
      <tr>
        <td>System Uptime</th>
        <td><span id="systemuptime">%datetime%</span></th>
        <td>Signal Strength</th>
        <td><span id="signalstrength">%datetime%</span></th>
        <td>Available RAM/Flash</th>
        <td><span id="ramflash">%datetime%</span></th>    
      </tr>
      <tr>
        <td>Core Temp</th>
        <td><span id="coretemp">%datetime%</span></th>
        <td>Access Point</th>
        <td><span id="accesspoint">%datetime%</span></th>
        <td>Bat. Voltage</th>
        <td><span id="batvoltage">%datetime%</span></th>    
      </tr>
      <tr>
        <td>Ext Voltage</th>
        <td><span id="extvoltage">%datetime%</span></th>
        <td></th>
        <td></th>
        <td>Bat. Current</th>
        <td><span id="batcurrent">%datetime%</span>...</th>    
      </tr>
    </table></p>
  </div>
  <script src="TrackMeasuringDisplay.js"></script>  
</body>
</html>)rawliteral";

AsyncWebSocket WebPageTrackMeasuring::_ws("/wstrackmeasuring");
AsyncEventSource WebPageTrackMeasuring::_events("/eventstrackmeasuring");
bool WebPageTrackMeasuring::_ledState = false;
unsigned long WebPageTrackMeasuring::_lastTime = 0;
unsigned long WebPageTrackMeasuring::_timerDelay = 1000;
float WebPageTrackMeasuring::_temperature = 0;
float WebPageTrackMeasuring::_humidity = 0;
float WebPageTrackMeasuring::_pressure = 0;
char WebPageTrackMeasuring::_wsTxBuffer[16384];
int WebPageTrackMeasuring::_millisRollOver = 0;
unsigned long WebPageTrackMeasuring::_lastMillis = 0;
String WebPageTrackMeasuring::BBVersion = "1.6.0";

void WebPageTrackMeasuring::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    WifiDebug::println("WebSocket event");

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;

        int docSize = 4096;
        DynamicJsonDocument doc(docSize);
        DeserializationError error = deserializeJson(doc, (char *)data);

        if (error)
        {
            WifiDebug::println("Error deserializing json data.");
            return;
        }

        if (doc.containsKey("Cmd") == false)
        {
            return;
        }

        String thisCmd = doc["Cmd"];
        WifiDebug::println(thisCmd);

        if (thisCmd == "SetSensor")
        {
            String subCmd = doc["SubCmd"];
            WifiDebug::println(subCmd);

            if (subCmd == "ClearDist")
            {
                PurpleHatModule::ResetDistance();
            }
            if (subCmd == "ClearHeading")
            {
                PurpleHatModule::ClearHeading();
            }
            if (subCmd == "RepRate")
            {

                uint16_t repRate = doc["Val"];
                PurpleHatModule::RepRate(repRate);
            }
        }
        if (thisCmd == "CfgData") // Config Request Format: {"Cmd":"CfgData", "Type":"pgxxxxCfg", "FileName":"name"}
        {
            String cmdType = doc["Type"];
            String fileName = doc["FileName"];
            
            WifiDebug::print("Request for config file: ");
            WifiDebug::println(fileName);

            String cmdMsg;
            cmdMsg.reserve(512);
            cmdMsg += "{\"Cmd\":\"CfgData\", \"ResetData\":true, \"Type\":\"";
            cmdMsg += cmdType; 
            cmdMsg += "\",\"Data\":";
            
            String configData;
            ConfigLoader::readFile("/configdata/" + fileName, configData);            
            cmdMsg += configData;
            cmdMsg += "}";
           
            _events.send(cmdMsg.c_str(),
                        "CfgData",
                         millis());
        }
        if (thisCmd == "STATS")
        {
            DynamicJsonDocument doc(640);
            //        char myStatusMsg[600];
            doc["Cmd"] = "STATS";
            JsonObject Data = doc.createNestedObject("Data");
            float float1 = (_millisRollOver * 4294967296) + millis(); // calculate millis including rollovers
            Data["uptime"] = round(float1);
            //  Data["uptime"] = millis();

            time_t now;
            now = time(0);
            //    char buff[40]; //39 digits plus the null char
            strftime(_wsTxBuffer, 40, "%m-%d-%Y %H:%M:%S", localtime(&now));
            Data["systime"] = _wsTxBuffer;

            Data["freemem"] = String(ESP.getFreeHeap());
            Data["totaldisk"] = String(SPIFFS.totalBytes());
            Data["useddisk"] = String(SPIFFS.usedBytes());
            Data["freedisk"] = String(SPIFFS.totalBytes() - SPIFFS.usedBytes());
            sprintf(_wsTxBuffer, "%s", BBVersion.c_str());
            Data["version"] = _wsTxBuffer;
            Data["ipaddress"] = WiFi.localIP().toString();
            Data["sigstrength"] = WiFi.RSSI();
            Data["apname"] = WiFi.SSID();

            Data["temp"] = M5.Power.Axp192.getInternalTemperature();
            Data["ubat"] = M5.Power.Axp192.getBatteryVoltage();
            Data["ibat"] = M5.Power.Axp192.getBatteryDischargeCurrent();
            Data["pwrbat"] = M5.Power.Axp192.getBatteryPower();
            Data["ubus"] = M5.Power.Axp192.getVBUSVoltage();
            Data["ibus"] = M5.Power.Axp192.getVBUSCurrent();
            Data["uin"] = M5.Power.Axp192.getACINVolatge();
            Data["iin"] = M5.Power.Axp192.getACINCurrent();

            serializeJson(doc, _wsTxBuffer);
            WifiDebug::println(_wsTxBuffer);
            _events.send(_wsTxBuffer,
                         "STATS",
                         millis());
        }
    }
}

void WebPageTrackMeasuring::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                                    void *arg, uint8_t *data, size_t len)
{ 
    switch (type)
    {
    case WS_EVT_CONNECT:           
        WifiDebug::print("WebSocket client #");
        WifiDebug::print(client->id());
        WifiDebug::print(" connected from ");
        WifiDebug::println(client->remoteIP().toString().c_str());        
        break;
    case WS_EVT_DISCONNECT:       
        WifiDebug::print("WebSocket client # ");
        WifiDebug::print(client->id());
        WifiDebug::println(" disconnected");        
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void WebPageTrackMeasuring::begin(AsyncWebServer *server)
{
    _ws.onEvent(onEvent);
    server->addHandler(&_ws);

    // Handle Web Server Events
    _events.onConnect([](AsyncEventSourceClient *client)
    {
        if(client->lastId())
        {
            WifiDebug::print("Client reconnected! Last message ID that it got is: ");
            WifiDebug::println(client->lastId());        
        }

        // send event with message "hello!", id current millis
        // and set reconnect delay to 1 second   
        client->send("hello!", "", millis(), 1000);
    });

    server->addHandler(&_events);

    // Route for root / web page
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
         request->send_P(200,
            "text/html",
                                 _html,
                                 processor); });

    server->on("/readings", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    String json = getSensorReadingsJSON();
    request->send(200, "application/json", json);
    json = String(); });
}

String WebPageTrackMeasuring::processor(const String& var)
{
    getSensorReadings();
    
    if (var == "TEMPERATURE")
    {
        return String(_temperature);
    }
    else if (var == "HUMIDITY")
    {
        return String(_humidity);
    }
    else if (var == "PRESSURE")
    {
        return String(_pressure);
    }
    return String();
}

void WebPageTrackMeasuring::loop()
{
    if (millis() < _lastMillis)
        _millisRollOver++; // update ms rollover counter
    else
        _lastMillis = millis();

    _ws.cleanupClients();
    if (_ws.count() == 0)
    {
        return;
    }

    if ((millis() - _lastTime) > _timerDelay)
    {
        getSensorReadings();
        char msg[50];
        snprintf(msg,
                 50,
                 "Temperature = %.2f ºC \n",
                 _temperature);
        WifiDebug::print(msg);

        snprintf(msg,
                 50,
                 "Humidity = %.2f \n",
                 _humidity);
        WifiDebug::print(msg);

        snprintf(msg,
                 50,
                 "Pressure = %.2f hPa \n",
                 _pressure);
        WifiDebug::print(msg);
        WifiDebug::println("-----");

        // Send Events to the Web Client with the Sensor Readings

        _events.send("ping", "", millis());
        _events.send(String(_temperature).c_str(), "temperature", millis());
        _events.send(String(_humidity).c_str(), "humidity", millis());
        _events.send(String(_pressure).c_str(), "pressure", millis());

        // send JSON object
        _events.send(getSensorReadingsJSON().c_str(), "new_readings", millis());

        // Get speed data
        String latestSpeedData;
        latestSpeedData.reserve(512);
        PurpleHatModule::GetSensorData(latestSpeedData);
        if (latestSpeedData.isEmpty() == false)
        {
            WifiDebug::println(latestSpeedData.c_str());
            _events.send(latestSpeedData.c_str(),
                         "SpeedData",
                         millis());
        }

        _lastTime = millis();
    }
}

void WebPageTrackMeasuring::getSensorReadings()
{
    _temperature += 1.0;
    _humidity += 2;
    _pressure += 3;
}

String WebPageTrackMeasuring::getSensorReadingsJSON()
{
    DynamicJsonDocument readings(1024);

    readings["sensor1"] = String(_temperature);
    readings["sensor2"] = String(_humidity);
    readings["sensor3"] = String(_pressure);
    readings["sensor4"] = String(0);

    String jsonString;
    serializeJson(readings, jsonString);
//    WifiDebug::println(jsonString);
    return jsonString;
}