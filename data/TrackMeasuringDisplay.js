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

window.addEventListener('load', onLoad);

var chart;

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
  console.log(data);
//  console.log(keys.length);  
    var x = (new Date()).getTime();
//    console.log(x);   
    var y = Number(data["Speed"]);
//    console.log(y);

   // if(chart.data.labels.length > 400) {
      chart.data.labels.push(x);
      chart.data.datasets[0].data.push(y);
    //} else {
    //  chartT.series[0].addPoint([x, y], true, false, true);
   // }
  chart.update();

  document.getElementById("measuredspeed").innerHTML = y;
  document.getElementById("absdistance").innerHTML = Number(data["AbsDist"]);
  document.getElementById("reldistance").innerHTML = Number(data["RelDist"]);

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
   
//  source.addEventListener('temperature', function(e) {
//     console.log("temperature", e.data);
//     document.getElementById("temp").innerHTML = e.data;
//    }, false);
   
   source.addEventListener('humidity', function(e) {
//    console.log("humidity", e.data);
    document.getElementById("scalespeed").innerHTML = e.data;
   }, false);
   
//    source.addEventListener('pressure', function(e) {
// //    console.log("pressure", e.data);
//     document.getElementById("pres").innerHTML = e.data;
//    }, false);
}

function onLoad(event) {
  initWebSocket();
  initChart();
}

function initChart(){
  ctx = document.getElementById("chart-speed-data").getContext("2d");
  chart = new Chart(ctx, {
    type: "line",
    data: {
      datasets: [{
        label: "Measured Speed [mm / s]",
        borderWidth: 1,
        pointRadius: 2,
        backgroundColor: '#C0FFC0',
        borderColor: '#C0FFC0'
      }],
    },
    options: {
      borderWidth: 3,
      borderColor: ["rgba(255, 99, 132, 128)"]           
    },
  });
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