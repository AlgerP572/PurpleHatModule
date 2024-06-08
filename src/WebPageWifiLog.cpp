#include "NTPTimeClient.h"
#include "WebPageWifiLog.h"
#include "WifiSerialDebug.h"

#include "PurpleHatModule.h"
#include "ConfigLoader.h"
#include "CpuUsage.h"

AsyncWebSocket WebPageWifiLog::_ws("/wswifilog");
AsyncEventSource WebPageWifiLog::_events("/eventswifilog");
unsigned long WebPageWifiLog::_lastTime = 0;
unsigned long WebPageWifiLog::_lastTimeDCC = 0;
unsigned long WebPageWifiLog::_timerDelay = 1000;
int WebPageWifiLog::_millisRollOver = 0;
unsigned long WebPageWifiLog::_lastMillis = 0;

std::vector<SerialCommandProcessor> _customSerialCommands;

void WebPageWifiLog::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    Log::println("WebSocket event", LogLevel::INFO);

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = '\0';       
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, reinterpret_cast<char*>(data));

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

        String thisCmd = doc["Cmd"].as<String>();
        Log::println(thisCmd, LogLevel::INFO);

        
        if (thisCmd == "LogCmd")
        {
            String subCmd = doc["SubCmd"].as<String>();
            Log::println(subCmd, LogLevel::DEBUG); 
            ProcessAllCommands(subCmd);
        } 
        if (thisCmd == "CfgData") // Config Request Format: {"Cmd":"CfgData", "Type":"pgxxxxCfg", "FileName":"name"}
        {
            String cmdType = doc["Type"].as<String>();
            String fileName = doc["FileName"].as<String>();
            
            Log::TakeMultiPrintSection();
            Log::print("Request for config file: ", LogLevel::INFO);
            Log::println(fileName, LogLevel::INFO);
            Log::GiveMultiPrintSection();

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
        if(thisCmd == "APPENDCOMMANDS")
        {
            // Add commands to the comboBox of the log view.
            WebPageWifiLog::SendCommandList(Log::GetCommands());
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

void WebPageWifiLog::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                                    void *arg, uint8_t *data, size_t len)
{ 
    switch (type)
    {
    case WS_EVT_CONNECT:
        Log::TakeMultiPrintSection();           
        Log::print("WebSocket client #", LogLevel::INFO);
        Log::print(client->id(), LogLevel::INFO);
        Log::print(" connected from ", LogLevel::INFO);
        Log::println(client->remoteIP().toString().c_str(), LogLevel::INFO);
        Log::GiveMultiPrintSection();       
        break;
    case WS_EVT_DISCONNECT:
        Log::TakeMultiPrintSection();       
        Log::print("WebSocket client # ", LogLevel::INFO);
        Log::print(client->id(), LogLevel::INFO);
        Log::println(" disconnected", LogLevel::INFO);
        Log::GiveMultiPrintSection();
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
        Log::TakeMultiPrintSection();   
        Log::print("WS Pong Id: ", LogLevel::DEBUG);
        Log::println(client->id(), LogLevel::DEBUG);
        Log::GiveMultiPrintSection();
        break;
    case WS_EVT_ERROR:
        Log::TakeMultiPrintSection();  
        Log::print("WS Error Id: ", LogLevel::ERROR);
        Log::println(client->id(), LogLevel::ERROR);
        Log::GiveMultiPrintSection();
        break;
    }
}

void WebPageWifiLog::begin(AsyncWebServer *server)
{
    if (server == nullptr)
    {
        // Handle the error appropriately       
        Log::println("Error: Server instance is null", LogLevel::ERROR);
        return;
    }

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
    server->on("/wifilog", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        // request->send_P(200,
        //     "text/html",
        //     _html,
        //     processor);

        request->send(SPIFFS,
            "/WifiLog.html",
            String(),
            false,
            processor);
    });   
}

String WebPageWifiLog::processor(const String& var)
{
    return String();
}

void WebPageWifiLog::AddCustomCommandProcessor(SerialCommandProcessor commandProcessor)
{
    _customSerialCommands.push_back(commandProcessor);
}

void WebPageWifiLog::SendCommandList(const std::vector<String> commands)
{

    String commandsData;
    commandsData.reserve(640);
    GetCommands(commandsData, commands);
            
    Log::println(commandsData, LogLevel::DEBUG);
    _events.send(commandsData.c_str(),
        "APPENDCOMMANDS",
        millis());
}

bool WebPageWifiLog::loop()
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

        _lastTime = millis();
    }

    if (Serial.available())
    {        
        String command = Serial.readStringUntil('\r');              
        ProcessAllCommands(command);
    }
    return true;
}

bool WebPageWifiLog::ProcessAllCommands(const String& command)
{
    SerialDebugCommands result = SerialDebugCommands::cmdNone;    
    result = Log::recvDebugDataMsg(command.c_str(), command.length());

    if(result == SerialDebugCommands::cmdNone)
    {
        for(std::vector<SerialCommandProcessor>::iterator it = _customSerialCommands.begin(); it != _customSerialCommands.end(); ++it)
        {
            bool extraResult = (*it)(command.c_str(), command.length());
            if(extraResult)
                result = SerialDebugCommands:: cmdCustom;
                break;
        }
    }
    else
    {
        String updateOptions;
        updateOptions.reserve(256);
        UpdateLogDisplayOptions(result, updateOptions);

         _events.send(updateOptions.c_str(),
            "DISPLAYUPDATE",
            millis());
    }
    return result;
}

void WebPageWifiLog::UpdateLogDisplayOptions(const SerialDebugCommands command, String& updateOptions)
{
    DynamicJsonDocument doc(1024);

    doc["Cmd"] = "DISPLAYUPDATE";
    JsonObject Data = doc["Data"].to<JsonObject>();

    switch(command)
    {
        case SerialDebugCommands::cmdSerialOn:
            Data["Serial"] = true;
        break;
        case SerialDebugCommands::cmdSerialOff:
            Data["Serial"] = false;
        break;
        case SerialDebugCommands::cmdWifiOn:
            Data["Wifi"] = true;
        break;
        case SerialDebugCommands::cmdWifiOff:
            Data["Wifi"] = false;
        break;

        case SerialDebugCommands::cmdDebugOn:
            Data["Debug"] = true;
        break;
        case SerialDebugCommands::cmdDebugOff:
            Data["Debug"] = false;
        break;
        case SerialDebugCommands::cmdLoopOn:
            Data["Loop"] = true;
        break;
        case SerialDebugCommands::cmdLoopOff:
            Data["Loop"] = false;
        break;
        case SerialDebugCommands::cmdErrorOn:
            Data["Error"] = true;
        break;
        case SerialDebugCommands::cmdErrorOff:
            Data["Error"] = false;
        break;
        case SerialDebugCommands::cmdInfoOn:
            Data["Info"] = true;
        break;
        case SerialDebugCommands::cmdInfoOff:
            Data["Info"] = false;
        break;

        case SerialDebugCommands::cmdTimeStampsOn:
            Data["TimeStamps"] = true;
        break;
        case SerialDebugCommands::cmdTImeStampsOff:
            Data["TimeStamps"] = false;
        break;
        case SerialDebugCommands::cmdPrintLevelOn:
            Data["PrintLevel"] = true;
        break;
        case SerialDebugCommands::cmdPrintLevelOff:
            Data["PrintLevel"] = false;
        break;
    }

    serializeJson(doc, updateOptions);
}

void WebPageWifiLog::GetStats(String& jsonData)
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

void WebPageWifiLog::GetCommands(String& jsonData, std::vector<String> commands)
{
    DynamicJsonDocument doc(1024);

    doc["Cmd"] = "Commands";
    JsonObject Data = doc["Data"].to<JsonObject>();
    JsonArray jsonCommands = Data["commands"].to<JsonArray>();

    for(int i = 0; i < commands.size(); i++)
    {
        jsonCommands.add(commands[i]);
    }

    serializeJson(doc, jsonData);
}


AsyncEventSource* WebPageWifiLog::GetEvents()
{    
    return &_events;
}