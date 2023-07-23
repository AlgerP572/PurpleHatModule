var gateway = `ws://${window.location.hostname}/wstrackmeasuring`;
var websocket;

function initWebSocket() {
  console.log('Trying to open a Track Measuring WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose; 
}

function onOpen(event) {
  console.log('Connection Track Measuring opened');
}
function onClose(event) {
  console.log('Connection Track Measuring closed');
  setTimeout(initWebSocket, 2000);
}

// Get current sensor readings when the page loads
window.addEventListener('load', onLoad);

// Create Temperature Chart
var chartT = new Highcharts.Chart({
  chart:{
    renderTo:'chart-speed-data'
  },
  series: [
    {
      name: 'Measured Speed [mm/s]',
      type: 'line',
      color: '#71B48D',
      marker: {
        symbol: 'circle',
        radius: 3,
        fillColor: '#71B48D',
      }
    },
    {
      name: 'Temperature #2',
      type: 'line',
      color: '#00A6A6',
      marker: {
        symbol: 'square',
        radius: 3,
        fillColor: '#00A6A6',
      }
    },
    {
      name: 'Temperature #3',
      type: 'line',
      color: '#8B2635',
      marker: {
        symbol: 'triangle',
        radius: 3,
        fillColor: '#8B2635',
      }
    },
    {
      name: 'Temperature #4',
      type: 'line',
      color: '#71B48D',
      marker: {
        symbol: 'triangle-down',
        radius: 3,
        fillColor: '#71B48D',
      }
    },
  ],
  title: {
    text: undefined
  },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: {
    title: {
      text: 'Current Speed [mm/s]'
    }
  },
  credits: {
    enabled: false
  }
});


//Plot temperature in the temperature chart
function plotTemperature(jsonValue) {

  var keys = Object.keys(jsonValue);
//  console.log(keys);
//  console.log(keys.length);

  for (var i = 0; i < keys.length; i++){
    var x = (new Date()).getTime();
//    console.log(x);
    const key = keys[i];
    var y = Number(jsonValue[key]);
//    console.log(y);

    if(chartT.series[i].data.length > 400) {
      chartT.series[i].addPoint([x, y], true, true, true);
    } else {
      chartT.series[i].addPoint([x, y], true, false, true);
    }

  }
}

function plotSpeed(jsonValue) {

  var data = jsonValue["Data"];
//  console.log(data);
//  console.log(keys.length);  
    var x = (new Date()).getTime();
//    console.log(x);   
    var y = Number(data["Speed"]);
//    console.log(y);

    if(chartT.series[0].data.length > 400) {
      chartT.series[0].addPoint([x, y], true, true, true);
    } else {
      chartT.series[0].addPoint([x, y], true, false, true);
    }
}

// Function to get current readings on the webpage when it loads for the first time
function getReadings(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj);
      plotTemperature(myObj);
    }
  };
  xhr.open("GET", "/readings", true);
  xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/eventstrackmeasuring');

  source.addEventListener('open', function(e) {
    console.log("Track Measuring Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Track Measuring Events Disconnected");
    }
  }, false);

  source.addEventListener('message', function(e) {
    //console.log("Track Measuring message", e.data);
  }, false);

//   source.addEventListener('new_readings', function(e) {
//     //console.log("Track Measuring new_readings", e.data);
//     var myObj = JSON.parse(e.data);
// //    console.log(myObj);
//     plotTemperature(myObj);
//   }, false);

  source.addEventListener('SpeedData', function(e) {
    //console.log("Track Measuring new_readings", e.data);
    var myObj = JSON.parse(e.data);
//    console.log(myObj);
    plotSpeed(myObj);
  }, false);
   
// source.addEventListener('temperature', function(e) {
//    console.log("temperature", e.data);
//    document.getElementById("temp").innerHTML = e.data;
//   }, false);
   
   source.addEventListener('humidity', function(e) {
//    console.log("humidity", e.data);
    document.getElementById("hum").innerHTML = e.data;
   }, false);
   
   source.addEventListener('pressure', function(e) {
//    console.log("pressure", e.data);
    document.getElementById("pres").innerHTML = e.data;
   }, false);
}

function onLoad(event) {
  initWebSocket();
}

function startMeasuring(sender)
{
  console.log("Click", sender);
	if (sender.innerHTML == "Start")
	{		
		websocket.send("{\"Cmd\":\"SetSensor\", \"SubCmd\":\"RepRate\",\"Val\":500}");
		document.getElementById("btnStart").innerHTML = "Stop";
	}
	else
	{
		websocket.send("{\"Cmd\":\"SetSensor\", \"SubCmd\":\"RepRate\",\"Val\":0}");
		document.getElementById("btnStart").innerHTML = "Start";
	}
}