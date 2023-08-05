var gateway = `ws://${window.location.hostname}/wsspeedmagic`;
var websocket;
var configData;
var statsData;

var	locoAddr = -1;
var locoAddrValid = false;

var cvVal_2 = 0;
var cvVal_3 = 0;
var cvVal_4 = 0;
var cvVal_5 = 0;
var cvVal_6 = 0;
var cvVal_29 = 0;

function initWebSocket() {
  console.log('Trying to open a Speed Magic WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose; 
}

function onOpen(event) {
  console.log('Connection Speed Magic opened');  
}
function onClose(event) {
  console.log('Connection Speed Magic closed');
  setTimeout(initWebSocket, 2000);
}

window.addEventListener('load', onLoad);

var chart;
var currentX = 0;

function plotSpeed(jsonValue) {

  var data = jsonValue.Data;
  console.log(data);
  var x = ++currentX; // (new Date()).getTime();
  var y = Number(data.Speed);

  document.getElementById("angle").innerHTML = data.AxisAngle.toFixed(2);    

     // if(chart.data.labels.length > 400) {
      chart.data.labels.push(x);
      chart.data.datasets[0].data.push(y);
      chart.data.datasets[1].data.push(scaleSpeed);
    //} else {
    //  chartT.series[0].addPoint([x, y], true, false, true);
   // }
  chart.update();
}

function  UpdateUI()
{
  document.getElementById("wheeldiameter").innerHTML = configData.WheelDia.toFixed(2);  
  document.getElementById("scaleunit").innerHTML = "1:" + configData.ScaleList[configData.ScaleIndex].Scale;
 
  chart.options.scales['y1'].title.text = 'Scale (' + configData.ScaleList[configData.ScaleIndex].Name + "  1:" + configData.ScaleList[configData.ScaleIndex].Scale + ') Speed [km/h]';
  chart.update();
}

function  UpdateFooter()
{
  document.getElementById("datetime").innerHTML = statsData.systime; 
  document.getElementById("ipaddress").innerHTML = statsData.ipaddress;
  document.getElementById("fwversion").innerHTML = statsData.version;
  
  document.getElementById("systemuptime").innerHTML = formatTime(Math.trunc(statsData.uptime/1000)); 
  document.getElementById("signalstrength").innerHTML = statsData.sigstrength  + " dBm";
  document.getElementById("ramflash").innerHTML = statsData.freemem + " / " + statsData.freedisk + " Bytes";

  document.getElementById("coretemp").innerHTML = statsData.temp.toFixed(2) + "\u00B0C"; 
  document.getElementById("accesspoint").innerHTML = statsData.apname;
  document.getElementById("batvoltage").innerHTML = statsData.ubat.toFixed(2) + " V";

  document.getElementById("extvoltage").innerHTML = statsData.uin.toFixed(2) + " V";
  document.getElementById("batcurrent").innerHTML = statsData.ibat.toFixed(2) + " mA";  
}

if (!!window.EventSource) {
  var source = new EventSource('/eventsspeedmagic');

  source.addEventListener('open', function(e) {
    console.log("Speed Magic Events Connected");    
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Speed Magic Events Disconnected");      
    }
  }, false);

  source.addEventListener('message', function(e) {
    //console.log("Speed Magic message", e.data);
  }, false);

  source.addEventListener('SpeedData', function(e) {
    //console.log("Speed Magic new_readings", e.data);
    var myObj = JSON.parse(e.data);
//    console.log(myObj);
    plotSpeed(myObj);
  }, false);

  source.addEventListener('CfgData', function(e) {
    //console.log("Speed Magic new_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj);
    configData = myObj["Data"];
    UpdateUI();
  }, false);

  source.addEventListener('STATS', function(e) {
    //console.log("Speed Magic new_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj);
    statsData = myObj["Data"];
    UpdateFooter();
  }, false);
}

function onLoad(event) {
  initWebSocket();
  getConfig();

  // This requires the prvious two.
  initChart();  
}

function initChart(){
  ctx = document.getElementById("chart-speed-data").getContext("2d");
  chart = new Chart(ctx, {
    type: "line",
    data: {
      datasets: [{
        label: "Measured Speed [mm/s]",
        borderWidth: 1,
        pointRadius: 2,
        backgroundColor: '#80C080',
        borderColor: '#80C080',
        yAxisID: 'y',
      },
      {
        label:"Scale Speed [km/h]",
        borderWidth: 1,
        pointRadius: 2,
        backgroundColor: '#8080C0',
        borderColor: '#8080C0',
        yAxisID: 'y1',
      }],
    },
    options: {
      borderWidth: 3,
      borderColor: ["rgba(255, 99, 132, 128)"],
      scales: {
        x: {
            title: {
                display: true,
                text: 'Samples [count]'
                }
        }, 
        y: {
            title: {
                display: true,
                text: 'Measured Speed [mm/s]'
            },
            type: 'linear',
            display: true,
            position: 'left',
        },
        y1: {
            title: {
                display: true,
                text: 'Scale Speed [km/h]'
            },
            type: 'linear',
            display: true,
            position: 'right',
  
          // grid line settings
          grid: {
            drawOnChartArea: false, // only want the grid lines for one axis to show up
          },
        }
      }           
    },    
  });
}

function loadJMRIDlg(sender)
{
    console.log("Click", sender);
    document.getElementById("load-btn").click();
}

function loadJMRIFile(sender)
{
    console.log("Load JMRI File", sender);
    loadJMRIDecoder(sender);
}

function loadJMRIDecoder(sender)
{
	fileName = sender.files[0];
	var reader = new FileReader();
	var parser;
    reader.onload = function()
    {
        try 
        {
			parser = new DOMParser();
			locoDef = parser.parseFromString(reader.result,"text/xml");
		}
		catch(err) 
		{
			fileName = "";
			alert("Configuration data not valid");
			return;
		}
        validLocoDef = displayDecoder(locoDef);
		setButtonStatus();
    };
    xmlFile = fileName;
    reader.readAsText(fileName);	
}

function displayDecoder(xmlNode)
{
	var isLoco = xmlNode.getElementsByTagName("locomotive-config").length > 0;
	var varVals = xmlNode.getElementsByTagName("varValue");
	var hasSpeedTable = -1;
	var cvVal = [];
	if (varVals.length > 0)
	{
		console.log(varVals.length);
		var attr = findXMLAttribute(varVals, "item", "Speed Table Definition");
		console.log(attr);
		if (attr)
			hasSpeedTable = attr.getAttribute("value");
		if (hasSpeedTable == 1)
		{
			cvVal = xmlNode.getElementsByTagName("CVvalue");
//			speedTableProfileGraph.LineGraphs[0].DataElements.length = 0;//delete old data
			var newEl = JSON.parse(JSON.stringify(DataElementTemplate));
			newEl.x = 0;
			newEl.y = 0;
//			speedTableProfileGraph.LineGraphs[0].DataElements.push(newEl);
			console.log(xmlNode);
			for (var i = 0; i < 28; i++) //28 table entries by definition
			{
				var cv = findXMLAttribute(cvVal, "name", (i+67).toString());
				if (cv)
				{
					newEl = JSON.parse(JSON.stringify(DataElementTemplate));
					newEl.x = i+1;
					newEl.y = cv.getAttribute("value");
//					speedTableProfileGraph.LineGraphs[0].DataElements.push(newEl);
				}
			}
		}
	}
//	drawProfileBox(canvasElementSpeedTable, speedTableProfileGraph);
	if (isLoco)
	{
		document.getElementById("jmrifilename").innerHTML = xmlNode.getElementsByTagName("locomotive")[0].getAttribute("fileName");
		document.getElementById("dccloco").innerHTML = xmlNode.getElementsByTagName("locomotive")[0].getAttribute("id");
		document.getElementById("jmriaddr").innerHTML = xmlNode.getElementsByTagName("locomotive")[0].getAttribute("dccAddress");
		document.getElementById("decbrand").innerHTML = findCVVal(xmlNode, 8);
		document.getElementById("decfamily").innerHTML = xmlNode.getElementsByTagName("decoder")[0].getAttribute("family");
		document.getElementById("decmodel").innerHTML = xmlNode.getElementsByTagName("decoder")[0].getAttribute("model");

		document.getElementById("cvn02").innerHTML = findCVVal(xmlNode, 2);
        document.getElementById("cvn03").innerHTML = findCVVal(xmlNode, 3);
        document.getElementById("cvn04").innerHTML = findCVVal(xmlNode, 4);

		document.getElementById("cvn05").innerHTML = findCVVal(xmlNode, 5);
		document.getElementById("cvn06").innerHTML = findCVVal(xmlNode, 6);
		document.getElementById("cvn29").innerHTML = findCVVal(xmlNode, 29);

		cvVal_2 = findCVVal(xmlNode, 2);
		cvVal_5 = findCVVal(xmlNode, 5);
		cvVal_6 = findCVVal(xmlNode, 6);
		cvVal_29 = findCVVal(xmlNode, 29);
		
		var vMax = xmlNode.getElementsByTagName("locomotive")[0].getAttribute("maxSpeed");		
		document.getElementById("jmrivmax").innerHTML = vMax;
//		setThrottleProfile(document.getElementById("thscalespeed"));
		var newAddr = xmlNode.getElementsByTagName("locomotive")[0].getAttribute("dccAddress");
		locoAddr = -1;
		reqDCCAssign(newAddr);
	}
	else
		console.log("Not Locomotive data");
	return isLoco;
}

function reqDCCAssign(forAddress)
{
	if (forAddress >= 0)
		websocket.send("{\"Cmd\":\"SetSensor\", \"SubCmd\":\"SetDCC\"," + " \"Addr\":" + forAddress.toString()+ " }");
	else
		websocket.send("{\"Cmd\":\"SetSensor\", \"SubCmd\":\"SetDCC\"}");
}


function findXMLAttribute(inCollection, thisAttr, attrVal)
{
	for (var i = 0; i < inCollection.length; i++)
	{
//		console.log(i, inCollection[i].getAttribute(thisAttr));
		if (inCollection[i].getAttribute(thisAttr) == attrVal)
			return inCollection[i];
	}
	return null;
}

function findCVVal(xmlNode, cvNr)
{
	var cvVals = xmlNode.getElementsByTagName("CVvalue");
	for (var i = 0; i < cvVals.length; i++)
		if (cvVals[i].getAttribute("name") == cvNr) 
			return cvVals[i].getAttribute("value");
	return -1;
}

function resetDistance(sender)
{
	sendMessage(websocket, "{\"Cmd\":\"SetSensor\", \"SubCmd\":\"ClearDist\"}");	
}

function getConfig()
{
  sendMessage(websocket, "{\"Cmd\":\"CfgData\", \"Type\":\"TODO:ID\", \"FileName\":\"phcfg.cfg\"}");
  sendMessage(websocket, "{\"Cmd\":\"STATS\"}");
}

function waitForOpenConnection(socket) {
  return new Promise((resolve, reject) => {
      const maxNumberOfAttempts = 10
      const intervalTime = 200 //ms

      let currentAttempt = 0
      const interval = setInterval(() => {
          if (currentAttempt > maxNumberOfAttempts - 1) {
              clearInterval(interval)
              reject(new Error('Maximum number of attempts exceeded'))
          } else if (socket.readyState === socket.OPEN) {
              clearInterval(interval)
              resolve()
          }
          currentAttempt++
      }, intervalTime)
  })
}

async function sendMessage (socket, msg)
{
  if (socket.readyState !== socket.OPEN) {
      try {
          await waitForOpenConnection(socket)
          socket.send(msg)
      } catch (err) { console.error(err) }
  } else {
      socket.send(msg)
  }
}

function formatTime(seconds) {
  return [
      parseInt(seconds / 60 / 60),
      parseInt(seconds / 60 % 60),
      parseInt(seconds % 60)
  ]
      .join(":")
      .replace(/\b(\d)\b/g, "0$1")
}
