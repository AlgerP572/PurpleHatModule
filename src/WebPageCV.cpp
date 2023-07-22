#include "WebPageCV.h"

char WebPageCv::_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Engine Address: 3</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;    
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  table {
    border-collapse: collapse;
    width: 100%;
  }
  th, td {
    text-align: left;
    padding: 8px;
  }
  tr:nth-child(even){
  background-color: #D6EEEE;
  }  
  </style>
<title>DCC over Wifi</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Configuration Variables for Engine Address: 3</h1>
  </div>
  <div class="content">
    <div class="card">
      <table>
        <tr>
          <th>CV</th>
          <th>Value</th>
          <th></th>
          <th></th>
          <th>Description</th>
        </tr>
        <tr>
          <td>1</td>
          <td>3</td>
          <td><button id="button">Read</button></td>
          <td><button id="button">Write</button></td>
          <td>Primary address</td>
        </tr>
         <tr>
          <td>2</td>
          <td>0</td>
          <td><button id="button">Read</button></td>
          <td><button id="button">Write</button></td>
          <td>V-Start</td>
        </tr>       
      </table>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>)rawliteral";

AsyncWebSocket WebPageCv::_ws("/ws");
bool WebPageCv::_ledState = false;

void WebPageCv::notifyClients()
{
  _ws.textAll(String(_ledState));
}

void WebPageCv::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
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

void WebPageCv::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void WebPageCv::begin(AsyncWebServer* server)
{
  _ws.onEvent(onEvent);
  server->addHandler(&_ws);

  // Route for root / web page
  server->on("/CV", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200,
     "text/html",
      _html,
       processor);
  });
}

String WebPageCv::processor(const String& var)
{
  Serial.println(var);
  if(var == "STATE")
  {
    if (_ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}
