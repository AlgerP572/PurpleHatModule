#include "NTPTimeClient.h"
#include "WebPageTrackMeasuring.h"
#include "WifiSerialDebug.h"
#include "CpuUsage.h"

#include "PurpleHatModule.h"
#include "ConfigLoader.h"

//char WebPageTrackMeasuring::_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE HTML><html>
// <head>
//   <title>Purple Hat Sensor</title>
//   <meta name="viewport" content="width=device-width, initial-scale=1">
//   <link rel="icon" href="data:,">
//   <link rel="stylesheet" type="text/css" href="style.css">
//   <script
//       src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.9.1/chart.min.js"
//       integrity="sha512-ElRFoEQdI5Ht6kZvyzXhYG9NqjtkmlkfYk0wr6wHxU9JEHakS7UJZNeml5ALk+8IKlU6jDgMabC3vkumRokgJA=="
//       crossorigin="anonymous"
//       referrerpolicy="no-referrer"
//     ></script>    
//   <style>
//     .topnav { background-color: #800080; }   
//     .reading { font-size: 1.4rem; }
//   </style>
// </head>
// <body>
//   <div class="topnav">
//     <div class="tool-bar">        
//         <a href="/">
//             <button class="btn-group">Track Data</button>
//         </a>
//         <a href="/speedmagic">
//             <button>Speed Magic</button>
//         </a>
//         <a href="/update">
//             <button>FW Update</button>
//         </a>
//         <a href="/webserial">
//             <button>Log</button>
//         </a>
//         <h1>Track Measuring Display</h1>
//     </div>
//   </div>
//   <div class="content">
//     <div class="card-grid">
//       <div class="card">
//         <div class="button-container">
//           <button id="btnStart" class="button" onClick="startMeasuring(this)">Start</button>
//           <button id="btnStart" class="button" onClick=" resetDistance(this)">Reset Distance</button>
//         </div>                   
//         <p><table class="datatable">
//         <tr>
//           <th>Data</th>
//           <th>Value</th>
//           <th>Units</th>          
//         </tr>
//          <tr>
//           <td>Scale</td>
//           <td><span id="scale">%Scale%</span></td>
//           <td><span id="scaleunit">%Scaleunit%</span></td>        
//         </tr>
//         <tr>
//           <td>Wheel Diameter</td>
//           <td><span id="wheeldiameter">%Wheel Diameter%</span></td>
//           <td>[mm]</td>          
//         </tr>
//          <tr>
//           <td>Angle</td>
//           <td><span id="angle">%angle%</span></td>
//           <td>[&deg]</td>          
//         </tr>
//          <tr>
//           <td>Reverse Direction</td>
//           <td><span id="reversedirection">%reversedirection%</span></td>
//           <td></td>          
//         </tr>
//         <tr>
//           <td>Direction</td>
//           <td><span id="direction">%Direction%</span></td>
//           <td></td>          
//         </tr>
//         <tr>
//           <td>Measured Speed</td>
//           <td><span id="measuredspeed">%Measured Soeed%</span></td>
//           <td>[mm/s]</td>          
//         </tr>        
//         <tr>
//           <td>Scale Speed</td>
//           <td><span id="scalespeed">%scalespeed%</span></td>
//           <td>[km/h]</td>
//         </tr>
//         <tr>
//           <td>Absolute Distance</td>
//           <td><span id="absdistance">%absdistance%</span></td>
//           <td>[mm]</td>  
//         </tr>
//         <tr>
//           <td>Relative Distance</td>
//           <td><span id="reldistance">%reldistance%</span></td>
//           <td>[mm]</td>  
//         </tr>
//          <tr>
//           <td>Sampling Rate</td>
//           <td><span id="samplingrate">%samplingrate%</span></td>
//           <td>[Hz]</td>  
//         </tr>
//       </table></p>
//       </div>      
//       <div class="card">
//         <div class="tool-bar-graph">
//           <button id="buttonCsv" class="profilebutton">To Csv</button>
//           <p class="card-title">Speed Data</p>
//         </div>
//           <canvas id="chart-speed-data" width="600" height="400"></canvas>
//         </div>
//     </div>
//   </div>
//   <div class="topnav">
//     <p><table class="footertable">
//       <tr>
//         <td>Date / Time</td>
//         <td><span id="datetime">%datetime%</span></td>
//         <td>IP Address</td>
//         <td><span id="ipaddress">%ipaddress%</span></td>
//         <td>Firmware Version</td>
//         <td><span id="fwversion">%fwversion%</span></td>    
//       </tr>
//       <tr>
//         <td>System Uptime / Cpu Load</td>
//         <td><span id="systemuptime">%dsystemuptime%</span></td>
//         <td>Signal Strength</td>
//         <td><span id="signalstrength">%signalstrength%</span></td>
//         <td>Available RAM/Flash</td>
//         <td><span id="ramflash">%ramflash%</span></td>    
//       </tr>
//       <tr>
//         <td>Core Temp</td>
//         <td><span id="coretemp">%coretemp%</span></td>
//         <td>Access Point</td>
//         <td><span id="accesspoint">%accesspoint%</span></td>
//         <td>Bat. Voltage</td>
//         <td><span id="batvoltage">%batvoltage%</span></td>    
//       </tr>
//       <tr>
//         <td>Ext Voltage</td>
//         <td><span id="extvoltage">%extvoltage%</span></td>
//         <td></td>
//         <td></td>
//         <td>Bat. Current</td>
//         <td><span id="batcurrent">%batcurrent%</span></td>    
//       </tr>
//     </table></p>
//   </div>
//     <script src="canvasjsascsv.min.js"></script> 
//     <script src="TrackMeasuringDisplay.js"></script>  
// </body>
// </html>)rawliteral";

AsyncWebSocket WebPageTrackMeasuring::_ws("/wstrackmeasuring");
AsyncEventSource WebPageTrackMeasuring::_events("/eventstrackmeasuring");
unsigned long WebPageTrackMeasuring::_lastTime = 0;
unsigned long WebPageTrackMeasuring::_timerDelay = 1000;
int WebPageTrackMeasuring::_millisRollOver = 0;
unsigned long WebPageTrackMeasuring::_lastMillis = 0;

void WebPageTrackMeasuring::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    Log::println("WebSocket event", LogLevel::INFO);

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;

        int docSize = 4096;
        DynamicJsonDocument doc(docSize);
        DeserializationError error = deserializeJson(doc, (char *)data);

        if (error)
        {
            Log::println("Error deserializing json data.", LogLevel::ERROR);
            return;
        }

        if (doc.containsKey("Cmd") == false)
        {
            Log::println("Cmd not provided nothing to do.", LogLevel::ERROR);
            return;
        }

        String thisCmd = doc["Cmd"];
        Log::println(thisCmd, LogLevel::INFO);

        if (thisCmd == "SetSensor")
        {
            String subCmd = doc["SubCmd"];
            Log::println(subCmd, LogLevel::INFO);

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
            
            Log::print("Request for config file: ", LogLevel::INFO);
            Log::println(fileName, LogLevel::INFO);

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
            String jsonData;
            jsonData.reserve(640);

            GetStats(jsonData);
            
            Log::println(jsonData, LogLevel::DEBUG);
            _events.send(jsonData.c_str(),
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
        Log::print("WebSocket client #", LogLevel::INFO);
        Log::print(client->id(), LogLevel::INFO);
        Log::print(" connected from ", LogLevel::INFO);
        Log::println(client->remoteIP().toString().c_str(), LogLevel::INFO);        
        break;
    case WS_EVT_DISCONNECT:       
        Log::print("WebSocket client # ", LogLevel::INFO);
        Log::print(client->id(), LogLevel::INFO);
        Log::println(" disconnected", LogLevel::INFO);        
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
            Log::print("Client reconnected! Last message ID that it got is: ", LogLevel::INFO);
            Log::println(client->lastId(), LogLevel::INFO);        
        }

        // send event with message "hello!", id current millis
        // and set reconnect delay to 1 second   
        client->send("hello!", "", millis(), 1000);
    });

    server->addHandler(&_events);

    // Route for root / web page
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        // request->send_P(200,
        //     "text/html",
        //     _html,
        //     processor);

        request->send(SPIFFS,
            "/TrackMeasuring.html",
            String(),
            false,
            processor);
    });   
}

String WebPageTrackMeasuring::processor(const String& var)
{
    return String();
}

bool WebPageTrackMeasuring::loop()
{
    if (millis() < _lastMillis)
        _millisRollOver++; // update ms rollover counter
    else
        _lastMillis = millis();

    if (_ws.count() == 0)
    {
        return false;
    }
    _ws.cleanupClients();

    bool result = false;
    if ((millis() - _lastTime) > _timerDelay)
    {
        Log::println("-----", LogLevel::LOOP);

        // Send Events to the Web Client with the Sensor Readings

        _events.send("ping", "", millis());

        String statsData;
        statsData.reserve(640);
        GetStats(statsData);
            
        Log::println(statsData, LogLevel::LOOP);
        _events.send(statsData.c_str(),
            "STATS",
            millis());

        // Get speed data
        String latestSpeedData;
        latestSpeedData.reserve(1024);
        PurpleHatModule::GetSensorData(latestSpeedData);
        if (latestSpeedData.isEmpty() == false)
        {
            Log::println(latestSpeedData.c_str(), LogLevel::LOOP);
            _events.send(latestSpeedData.c_str(),
                         "SpeedData",
                         millis());
        }

        _lastTime = millis();
        result = true;
    }

    return result;
}

void WebPageTrackMeasuring::GetStats(String& jsonData)
{
    DynamicJsonDocument doc(640);

    doc["Cmd"] = "STATS";
    JsonObject Data = doc.createNestedObject("Data");
    float float1 = (_millisRollOver * 4294967296) + millis(); // calculate millis including rollovers
    Data["uptime"] = round(float1);
   
    String formattedTime;
    TimeClient::getFormattedDate(formattedTime);
    Data["systime"] = formattedTime;
    Data["cpuload"] = CpuUsage::GetCpuLoad();
    Data["freemem"] = String(ESP.getFreeHeap());
    Data["totaldisk"] = String(SPIFFS.totalBytes());
    Data["useddisk"] = String(SPIFFS.usedBytes());
    Data["freedisk"] = String(SPIFFS.totalBytes() - SPIFFS.usedBytes());
    Data["version"] = VERSION;
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

    serializeJson(doc, jsonData);
}