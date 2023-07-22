#include "WebPageRoot.h"

char WebPageRoot::_html[] PROGMEM = R"rawliteral(
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
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }

  .button2 {background-color: #4CAF50;} /* Green */
  .button3 {background-color: #f44336;} /* Red */ 
  .button4 {background-color: #e7e7e7; color: black;} /* Gray */ 
  .button5 {background-color: #555555;} /* Black */

   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   } 
  </style>
<title>DCC over Wifi</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Test Engine Address: 3</h1>
  </div>
  <div class="content">
    <div class="card">
      <p><table>
        <tr>
          <td>
            <label>Speed</label>
          </td>
          <td colspan="2">
            <input type="range" min="0" max="128" value="0" id="throttle" step="1" list="speedsteps"
                oninput="document.getElementById('currentSpeedStep').innerHTML = this.value" />              
            <datalist id="speedsteps">
              <option>0</option>
              <option>16</option>
              <option>32</option>
              <option>48</option>
              <option>64</option>
              <option>80</option>
              <option>96</option>
              <option>112</option>
              <option>128</option>
            </datalist>
          </td>
          <td>
            <label id="currentSpeedStep"></label>
          </td>
        </tr>             
        <tr>
          <td>
             <p><button id="button" class="button button2">Reverse</button></p>
          </td>
          <td>
             <p><button id="button" class="button button3">Stop</button></p>
          </td>
           <td>
             <p><button id="button" class="button button3">E-Stop</button></p>
          </td>            
          <td>
             <p><button id="button" class="button button2">Forward</button></p>
          </td>
        </tr>        
      </table></p>       

      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
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

AsyncWebSocket WebPageRoot::_ws("/ws");
bool WebPageRoot::_ledState = false;

void WebPageRoot::notifyClients()
{
  _ws.textAll(String(_ledState));
}

void WebPageRoot::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
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

void  WebPageRoot::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
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

void WebPageRoot::begin(AsyncWebServer* server)
{
  _ws.onEvent(onEvent);
  server->addHandler(&_ws);

  // Route for root / web page
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200,
     "text/html",
      _html,
       processor);
  });
}

String WebPageRoot::processor(const String& var)
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