#include "NTPTimeClient.h"
#include "WebPageSpeedMagic.h"
#include "WifiSerialDebug.h"

#include "PurpleHatModule.h"
#include "ConfigLoader.h"

char WebPageSpeedMagic::_html[] PROGMEM = R"rawliteral(
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
    <div class="tool-bar">        
        <a href="/">
            <button class="btn-group">Track Data</button>
        </a>
        <a href="/speedmagic">
            <button>Speed Magic</button>
        </a>
        <a href="/update">
            <button>FW Update</button>
        </a>
        <a href="/webserial">
            <button>Log</button>
        </a>
        <h1>Speed Magic</h1>
    </div>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <p><table class="datatable">
             <tr>
                <th>Input:</th>
                <th></th>
                <th></th>
                <th></th>
                <th></th>
                <th></th>
            </tr>
             <tr>
                <td>JMRI File:</td>               
                <td>
                    <div class="tool-bar-graph"> 
                        <input type="file" id="load-btn" style="display:none" onChange="loadJMRIFile(this)"/>
                        <button id="load-button" onClick="loadJMRIDlg(this)">Load</button>
                        <button onClick=" resetDistance(this)">Save</button>
                    </div>
                </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
            </tr>
            <tr>
                <td>Throttle:</td>               
                <td>
                    <div class="tool-bar-graph"> 
                        <input type="file" id="load-profile-btn" style="display:none" onChange="loadThrottle(this)"/>
                        <button id="load-profile-button" onClick="loadThrottleDlg(this)">Load</button>
                        <button id="button">Save</button>
                    </div>
                </td>
                <td>Speed Step</td>
                <td><span id="speedstep">%coretemp%</span></td>
                <td>Direction</td>
                <td><span id="direction">%direction%</span></td>
            </tr>
            <tr>
                <td>Speed Test:</td>
                <td>
                    <div class="tool-bar-graph"> 
                        <button id="start-speed-test"  onClick="startSpeedTest(this)">Start</button>
                        <button id="stop-speed-test" onClick="stopSpeedTest(this)">Stop</button>
                    </div>
                </td>
                <td><span>Length [mm]</span></td>
                <td><input type="number" min="1" max="50000" value="13500" onchange="setTrackLength(this)"/></td>
                <td><span>v Max [km/h]</td>
                <td><input id="vMax" type="number" min="1" max="150" value="50" onchange="setTestSpeed(this)"/></td>
            </tr>        
        <tr>
            <th colspan="3">Data</th>
            <th colspan="3">Value</th>        
        </tr>
         <tr>
          <td colspan="3">File Name</td>
          <td colspan="3"><span id="jmrifilename">%fileName%</span></td>                  
        </tr>
        <tr>
          <td colspan="3">Locomotive</td>
          <td colspan="3"><span id="dccloco">%locomotive%</span></td>                   
        </tr>
         <tr>
          <td colspan="3">DCC Address</td>
          <td colspan="3"><span id="jmriaddr">%dccaddress%</span></td>
        </tr>
         <tr>
          <td colspan="3">Decoder Brand</td>
          <td colspan="3"><span id="decbrand">%brand%</span></td>
        </tr>
        <tr>
          <td colspan="3">Decoder Family</td>
          <td colspan="3"><span id="decfamily">%Direction%</span></td>
        </tr>
        <tr>
          <td colspan="3">Decoder Model</td>
          <td colspan="3"><span id="decmodel">%Measured Soeed%</span></td>
        </tr>        
        <tr>
          <td><span id="cv02">CV 02</span></td>
          <td><span id="cvn02">%cvvalue%</span></td>
          <td><span id="cv03">CV 03</span></td>
          <td><span id="cvn03">%cvvalue%</span></td>
          <td><span id="cv04">CV 04</span></td>
          <td><span id="cvn04">%cvvalue%</span></td>          
        </tr>
        <tr>          
          <td><span id="cv05">CV 05</span></td>
          <td><span id="cvn05">%cvvalue%</span></td>
          <td><span id="cv06">CV 06</span></td>
          <td><span id="cvn06">%cvvalue%</span></td>
          <td><span id="cv29">CV 29</span></td>
          <td><span id="cvn29">%cvvalue%</span></td> 
        </tr>
        <tr>
          <td>Scale</td>          
          <td><span id="scaleunit">%Scaleunit%</span></td>
          <td>Diameter [mm]</td>
          <td><span id="wheeldiameter">%Wheel Diameter%</span></td>
          <td>V Max [km/h]</td>
          <td><span id="jmrivmax">%maxspeed%</span></td>
        </tr>
      </table></p>
      </div>      
      <div class="card">
            <div class="tool-bar-graph">
                <button id="testCsv" class="profilebutton">To Csv</button>
                <input type="file" id="load-speedData-btn" style="display:none" onChange="loadSpeedData(this)"/>
                <button id="importCsv" class="profilebutton" onClick="loadSpeedDataDlg(this)">Ld Csv</button>
                <button id="calculate" class="profilebutton" onClick="calcTable(this)">Calc</button>
                <p>Profile Test</p>
            </div>            
            <canvas id="chart-speed-data" width="600" height="400"></canvas>
            <p><table class="datatable">
                <tr>
                    <th>FW trim</th>
                    <th>Speed table</th>
                    <th>BW trim</th>  
                </tr>
                <tr>
                    <td><span id="fwTrim" style='font-size: 8pt;'>%fwTrim%</span></td>
                    <td><span id="speedTable" style='font-size: 8pt;'>%speedTable%</span></td>
                    <td><span id="bwTrim" style='font-size: 8pt;'>%bwTrim%</span></td>
                </tr>
            </table></p>        
      </div>
    </div>
  </div>
  <div class="topnav">
    <p><table class="footertable">
      <tr>
        <td>Date / Time</th>
        <td><span id="datetime">%datetime%</span></th>
        <td>IP Address</th>
        <td><span id="ipaddress">%ipaddress%</span></th>
        <td>Firmware Version</th>
        <td><span id="fwversion">%fwversion%</span></th>    
      </tr>
      <tr>
        <td>System Uptime</th>
        <td><span id="systemuptime">%dsystemuptime%</span></th>
        <td>Signal Strength</th>
        <td><span id="signalstrength">%signalstrength%</span></th>
        <td>Available RAM/Flash</th>
        <td><span id="ramflash">%ramflash%</span></th>    
      </tr>
      <tr>
        <td>Core Temp</th>
        <td><span id="coretemp">%coretemp%</span></th>
        <td>Access Point</th>
        <td><span id="accesspoint">%accesspoint%</span></th>
        <td>Bat. Voltage</th>
        <td><span id="batvoltage">%batvoltage%</span></th>    
      </tr>
      <tr>
        <td>Ext Voltage</th>
        <td><span id="extvoltage">%extvoltage%</span></th>
        <td></th>
        <td></th>
        <td>Bat. Current</th>
        <td><span id="batcurrent">%batcurrent%</span></th>    
      </tr>
    </table></p>
  </div>
  <script src="regression.js"></script>
  <script src="canvasjsascsv.min.js"></script>
  <script src="papaparse.min.js"></script> 
  <script src="SpeedMagicDisplay.js"></script>  
</body>
</html>)rawliteral";

AsyncWebSocket WebPageSpeedMagic::_ws("/wsspeedmagic");
AsyncEventSource WebPageSpeedMagic::_events("/eventsspeedmagic");
unsigned long WebPageSpeedMagic::_lastTime = 0;
unsigned long WebPageSpeedMagic::_lastTestTime = 0;
unsigned long WebPageSpeedMagic::_timerDelay = 1000;
int WebPageSpeedMagic::_millisRollOver = 0;
unsigned long WebPageSpeedMagic::_lastMillis = 0;
String WebPageSpeedMagic::_latestSpeedTableData = "";
String WebPageSpeedMagic::BBVersion = "1.6.0";

void WebPageSpeedMagic::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
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

            if (subCmd == "SetDCC")
            {
                int16_t dccAddr = doc["Addr"];
                Log::print("DCC Addr: ", LogLevel::INFO);
                Log::println(dccAddr, LogLevel::INFO);

                PurpleHatModule::SetDccAddr(dccAddr);          
            } 
            if(subCmd == "RunTest")           
            {               
                float tLen = doc["TrackLen"];
                float vMax = doc["VMax"];
                uint8_t pMode = doc["Mode"];

                PurpleHatModule::startTest(tLen, vMax, pMode);
            }
            if(subCmd == "StopTest")
            {  
                String speedTableData;
                speedTableData.reserve(4096); // ??
                PurpleHatModule::stopTest(speedTableData);

                 _events.send(speedTableData.c_str(),
                    "SpeedTableData",
                    millis());
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

void WebPageSpeedMagic::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
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

void WebPageSpeedMagic::begin(AsyncWebServer *server)
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
    server->on("/speedmagic", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send_P(200,
            "text/html",
            _html,
            processor);
    });

    // shared between speed magic test and web event to publish.
    _latestSpeedTableData.reserve(4096);
}

String WebPageSpeedMagic::processor(const String& var)
{
    return String();
}

void WebPageSpeedMagic::loop()
{
    if (millis() < _lastMillis)
        _millisRollOver++; // update ms rollover counter
    else
        _lastMillis = millis();

    if (_ws.count() == 0)
    {
        return;
    }
    _ws.cleanupClients();
    delay(2);

    if((millis() - _lastTestTime) > speedTestInterval)
    {
        // Get speed data     
        _latestSpeedTableData.clear();  
        PurpleHatModule::GetSpeedTestData(_latestSpeedTableData);       

        // Send latest speed data to web.  This should happen at a much slower
        // rate. (See the speed test state machine.)
        if (_latestSpeedTableData.isEmpty() == false)
        {
            Log::println(_latestSpeedTableData.c_str(), LogLevel::LOOP);
            _events.send(_latestSpeedTableData.c_str(),
                         "SpeedTableData",
                         millis());
        }
    }

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
    }
}

void WebPageSpeedMagic::GetStats(String& jsonData)
{
    DynamicJsonDocument doc(640);

    doc["Cmd"] = "STATS";
    JsonObject Data = doc.createNestedObject("Data");
    float float1 = (_millisRollOver * 4294967296) + millis(); // calculate millis including rollovers
    Data["uptime"] = round(float1);

   
    String formattedTime;
    TimeClient::getFormattedDate(formattedTime);
    Data["systime"] = formattedTime;
    Data["freemem"] = String(ESP.getFreeHeap());
    Data["totaldisk"] = String(SPIFFS.totalBytes());
    Data["useddisk"] = String(SPIFFS.usedBytes());
    Data["freedisk"] = String(SPIFFS.totalBytes() - SPIFFS.usedBytes());
    Data["version"] = BBVersion;
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