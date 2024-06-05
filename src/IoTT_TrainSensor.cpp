/*

SoftwareSerial.cpp - Implementation of the Arduino software serial for ESP8266.
Copyright (c) 2015-2016 Peter Lerup. All rights reserved.

Adaptation to LocoNet (half-duplex network with DCMA) by Hans Tanner. 
See Digitrax LocoNet PE documentation for more information

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is dstributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <IoTT_TrainSensor.h>
#include "WifiSerialDebug.h"

//txFct sensorCallback = NULL;

void myTask(void * thisParam)
{
	IoTT_TrainSensor * thisObj = (IoTT_TrainSensor*)thisParam;
	while(1)
	{
//		Serial.printf("ST");		
		thisObj->sensorTask(NULL);

		// feed watch dog
//		delay(2);
	}
}

IoTT_TrainSensor::IoTT_TrainSensor(TwoWire* newWire, movingAvg* avgDistance, movingAvg* avgDistanceNoOUtliers, movingAvg* speedData, movingAvg* scaleSpeedData, movingAvg* scaleAccelData,
                                                     LowPass<2>* speedFilter,  LowPass<2>* accelFilter, uint8_t sda, uint8_t scl)
{
	sensorWire = newWire;
    _avgDistance = avgDistance;
    _avgDistanceNoOUtliers = avgDistanceNoOUtliers;
    _speedData = speedData;
    _scaleSpeedData = scaleSpeedData;
    _scaleAccelData = scaleAccelData;

    _lpSpeed = speedFilter;
    _lpAccel = accelFilter;
//	sensorWire->begin(sda, scl);
//	delay(10);
}

IoTT_TrainSensor::~IoTT_TrainSensor() 
{
	if (taskHandleSensor)
		vTaskDelete(taskHandleSensor);
}

volatile void IoTT_TrainSensor::sensorTask(void * thisParam)
{
	float rotAngle = -1;
	if ((millis() - lastSampleCtrl) > workData.measuringInterval)
	{
		uint32_t currTime = micros();
		lastSampleCtrl += workData.measuringInterval;

		if (magSensor)
		{

			if (!magCalibrated)
			{
				magSensor->collectCalibData();
				if ((abs(revCtr) > 3))
				{
					magSensor->setFluxOffset();
					workData.avgMove = 0;
					workData.absIntegrator = 0;
					workData.relIntegrator = 0;
					magCalibrated = true;
				}
			}

			if (reverseDir)
				rotAngle = magSensor->getAngleData();
			else
				rotAngle = 360 - magSensor->getAngleData();
		}
		else
			return; //no sensor, so don't do anything

		int8_t overFlow = 0;
		float_t relMove = rotAngle - workData.axisAngle;

		if (abs(rotAngle - workData.axisAngle) > 180)
		{
			overFlow = rotAngle > workData.axisAngle ? overFlow = -1 : overFlow = 1;
			if (overFlow > 0)
				relMove += (float_t)magOverflow;
		 	else
		 		relMove -= (float_t)magOverflow;
		}
		else
        {
		 	relMove = (rotAngle  - workData.axisAngle);
        }

		bool dirFwd = relMove >= 0;

		//workData.avgMove = (0.99 * workData.avgMove) + (0.01 * relMove); //used to detect standstill
        workData.avgMove = _avgDistance->reading(relMove);

        float stdDev = _avgDistance->getStdDev(_avgDistance->getCount());
        float zScore = (relMove - workData.avgMove) / stdDev;

        float sigma = 1.0;
        if(abs(zScore) < sigma)
        {
            workData.avgMoveNoOutliers = _avgDistanceNoOUtliers->reading(relMove);
        }
       
		bool isMoving = (abs(workData.avgMove) > 0.2);
		// if(isMoving)
		// {
		// //  	Serial.printf("Angle: %.2f AxislMove: %.2f RelMove: %.2f \n", rotAngle, workData.axisAngle, relMove);
        //     if(workData.currSpeedTech > 620 && workData.currSpeedTech < 650)
        //         Serial.printf("relMove: %.2f AvgMove: %.2f No outliers: %.2f stdDev: %.2f\n", relMove, workData.avgMove, workData.avgMoveNoOutliers, stdDev);
		// }
		revCtr += overFlow;     
		
		float_t linDistance = 0;
		float_t avgDistance = 0; //used for speed calculation
		
		//calculate time since last speed calculation
		uint32_t speedDiff = currTime - lastSpeedTime;
		workData.currDirFwd = dirFwd;
		workData.axisAngle = rotAngle;

        // How fast is it actually running? i.e is it achieving measurement interval?
        workData.avgTimeInterval =  (0.99f * workData.avgTimeInterval) + (0.01f * speedDiff);

		//calculate travelled distance
		linDistance = relMove * wheelDia * PI / 360.0f;
		avgDistance = workData.avgMoveNoOutliers * wheelDia * PI / 360.0f;
		if (isMoving)
        {
            float currentSpeedSample = avgDistance * 1000000.0f / speedDiff;
            float currentScaleSpeed = currentSpeedSample * 36.0f * workData.modScale / 10000.0f;            
           
			//workData.currSpeedTech = getFractionHigh(workData) * workData.currSpeedTech + getFractionLow(workData) * currentSpeedSample; // time is in µsec
            float xsn = _speedData->reading(currentSpeedSample);
            float ysn = _lpSpeed->filt(xsn);
            workData.currSpeedTech = ysn; 
            //float newScaleTech =   getFractionHigh(workData) * workData.currScaleTech + (getFractionLow(workData) * currentScaleSpeed);
            float newScaleSeedTech =  _scaleSpeedData->reading(currentScaleSpeed);

            float speedDelta = 1.0;
            float bufferTimeFactor = -1.330f * speedDelta + 10.0f; // From dynamic buffer xls file.
            float newBufferTime = (workData.maxBufferTime - 20.0f * workData.measuringInterval)/(1.0f + exp(-bufferTimeFactor)) + (workData.measuringInterval * 20.0f);
           
            float scaleAccel = (newScaleSeedTech - workData.currScaleSpeedTech) * 1000000.0f / speedDiff;            

            // updtae current stats.
            //workData.currScaleAccelTech =  (0.998f *  workData.currScaleAccelTech) + (0.002f * deltaSpeed);
            float xan =  _scaleAccelData->reading(scaleAccel);
            float yan = _lpAccel->filt(xan);
            workData.currScaleAccelTech = yan;
            //Serial.printf("Scale speed: %.2f Accel: %.2f Buffer : %.2f\n", workData.currScaleSpeedTech, workData.currScaleAccelTech, workData.bufferTime);
            workData.bufferTime = newBufferTime;
            workData.currScaleSpeedTech =  newScaleSeedTech;
        }
		else
        {
			workData.currSpeedTech = 0;           
            workData.currScaleSpeedTech = 0;
            workData.currScaleAccelTech = 0;

            float x0 = _speedData->reading(0);
            _lpSpeed->filt(x0);
            _scaleSpeedData->reading(0);
            float a0 = _scaleAccelData->reading(0);
            _lpAccel->filt(0);
        }
				
		lastSpeedTime = currTime;
		if (isMoving)
        {
			if (workData.avgMove > 0)
				workData.absIntegrator += linDistance;
			else
				workData.absIntegrator -= linDistance;
        }
		workData.relIntegrator += linDistance;

		//here we know travel distance and speed, so we can now read IMU and calculate vector and position, but only if distance > 0
		if (imuSensor)
		{
			if (!imuSensor->getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER)) //get euler vector m IMU
				Serial.println("no event");
			imuSensor->getCalibration(&mySystem, &myGyro, &myAccel, &myMag); //read calibration status as well
		}
		if (reloadOffset)
		{
			headingOffset[0] = -orientationData.orientation.x;
			headingOffset[1] = -orientationData.orientation.y;
			headingOffset[2] = -orientationData.orientation.z;
			reloadOffset = false;
		}

		if (abs(linDistance) > 0)
		{ 
			compOrientationData[0] = orientationData.orientation.x + headingOffset[0]; //yaw
			if (compOrientationData[0] < 0) compOrientationData[0] += 360;
			if (compOrientationData[0] > 360) compOrientationData[0] -= 360;

			compOrientationData[1] = orientationData.orientation.y + headingOffset[1]; //roll
			if (compOrientationData[1] > 90) compOrientationData[1] = 180 - compOrientationData[1];
			if (compOrientationData[1] < -90) compOrientationData[1] = -180 - compOrientationData[1];

			compOrientationData[2] = orientationData.orientation.z + headingOffset[2]; //pitch
			if (compOrientationData[2] < -180) compOrientationData[2] = 360 + compOrientationData[2];
			if (compOrientationData[2] > 180) compOrientationData[2] = -360 + compOrientationData[2];


			
			//convert euler angles to rad
			workData.eulerVectorRad[0] = compOrientationData[0] * TWO_PI  / 360;
			workData.eulerVectorRad[1] = compOrientationData[1] * TWO_PI  / 360;
			workData.eulerVectorRad[2] = compOrientationData[2] * TWO_PI  / 360;
			workData.eulerVectorRad[3] = linDistance;

			if (sin(workData.eulerVectorRad[2]) < 0.01) //min 1% slope
				workData.eulerVectorRad[2] = 0; //otherwise set to 0

			//add to position vector
			workData.posVector_mm[0] -= workData.eulerVectorRad[3] * cos(workData.eulerVectorRad[2]) * cos(workData.eulerVectorRad[0]);
			workData.posVector_mm[1] += workData.eulerVectorRad[3] * cos(workData.eulerVectorRad[2]) * sin(workData.eulerVectorRad[0]);
			workData.posVector_mm[2] += workData.eulerVectorRad[3] * sin(workData.eulerVectorRad[2]);

//			if ((currSpeed != 0) && (overFlow != 0)) //we are moving
			if ((overFlow != 0)) //we are moving
			{ //calculate curve radius
				float_t angleDiff = 0;
				if (abs(compOrientationData[0] - lastOrientationData[0]) > 180)
				{
					float yawOverFlow = compOrientationData[0] > lastOrientationData[0] ? yawOverFlow = -1 : yawOverFlow = 1;
					angleDiff = yawOverFlow > 0 ? compOrientationData[0] + 360 - lastOrientationData[0] : compOrientationData[0] - 360 - lastOrientationData[0];
				}
				else
					angleDiff = (compOrientationData[0] - lastOrientationData[0]);

					
				if (abs(angleDiff) > 0)
					workData.currRadiusTech = 180 * wheelDia / angleDiff;
				else
					workData.currRadiusTech = 0;
//				Serial.printf("%.2f %.2f  \n", angleDiff, workData.currRadiusTech);
//          Serial.println(workData.currRadiusTech);
				memcpy(&lastOrientationData, &compOrientationData, sizeof(compOrientationData));
			}

		}
		if (xSemaphoreTake(sensorSemaphore, portMAX_DELAY) == pdPASS) 
		{
			workData.timeStamp = millis();
			dispData = workData;
			xSemaphoreGive(sensorSemaphore);
		}
	}
}

float IoTT_TrainSensor::getFractionLow(sensorData sensorData)
{
    return  (sensorData.measuringInterval * 0.001f) / (sensorData.bufferTime * 0.001f);
}
float IoTT_TrainSensor::getFractionHigh(sensorData sensorData)
{
    return (1.f - getFractionLow(sensorData));
}

void IoTT_TrainSensor::begin() 
{
//	speedEstimate = new OneDimKalman(8,10,10,10);
	relMoveEstimate = new OneDimKalman(8,10,10,10);
	if (magType == 1)
	{
		Serial.println("Initialize TMAG5273");
		magSensor = new TMAG5273(sensorWire);
		if (!magSensor->setDeviceConfig(mountStyle))
		{
			/* There was a problem detecting the BNO055 ... check your connections */
			Serial.println("Ooops, no TMAG5273 detected ... Check your hardware!");
			delete magSensor;
			magSensor = NULL;
		}
	}
	if (imuType == 1)
	{
		Serial.println("Initialize BNO055");
		imuSensor = new Adafruit_BNO055(55, 0x29);
		if (!imuSensor->begin())
		{
			/* There was a problem detecting the BNO055 ... check your connections */
			Serial.println("Ooops, no BNO055 detected ... Check your hardware!");
			delete imuSensor;
			imuSensor = NULL;
		}
		else
		{
			displayIMUSensorDetails();
			displayIMUSensorStatus();
		}
	}
	if (!imuSensor) //use Stick imu sensor
	{
		Serial.println("Using internal IMU!");
	}
		
	sensorSemaphore = xSemaphoreCreateMutex();
	if (magSensor || imuSensor)
	{
		if (taskHandleSensor == NULL)
		{
			Serial.println("Create Sensor Task");
//			xTaskCreatePinnedToCore(myTask, 
			xTaskCreate(myTask,        ///* Task function. 
					"SensorTask",      //* String with name of task. 
                    16000,            //* Stack size in bytes. 
                    (void*)this,     //* Parameter passed as input of the task 
                    1,                //* Priority of the task. 
                    &taskHandleSensor);            //* Task handle, core Number
        }
	}
}

void IoTT_TrainSensor::displayIMUSensorDetails()
{
	if (imuSensor)
	{
		Adafruit_BNO055::adafruit_bno055_rev_info_t myInfo;
		sensor_t sensor;
		imuSensor->getRevInfo(&myInfo);
		imuSensor->getSensor(&sensor);
		Serial.println("------------------------------------");
		Serial.print("Sensor:       "); Serial.println(sensor.name);
		Serial.print("Driver Ver:   "); Serial.println(sensor.version);
		Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
		Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
		Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
		Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
		Serial.printf("Firmware:     %X.%2X\n", myInfo.sw_rev >> 8, myInfo.sw_rev & 0x00FF); 
		Serial.println("------------------------------------");
		Serial.println("");
	}
	else
		Serial.print("IMU Sensor not available");
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
    */
/**************************************************************************/
void IoTT_TrainSensor::displayIMUSensorStatus()
{
	if (imuSensor)
	{
		/* Get the system status values (mostly for debugging purposes) */
		uint8_t system_status, self_test_results, system_error;
		system_status = self_test_results = system_error = 0;
		imuSensor->getSystemStatus(&system_status, &self_test_results, &system_error);

		/* Display the results in the Serial Monitor */
		Serial.println("");
		Serial.print("System Status: 0x");
		Serial.println(system_status, HEX);
		Serial.print("Self Test:     0x");
		Serial.println(self_test_results, HEX);
		Serial.print("System Error:  0x");
		Serial.println(system_error, HEX);
		Serial.println("");
	}
	else
		Serial.print("IMU Sensor not available");
}

void IoTT_TrainSensor::loadLNCfgJSON(DynamicJsonDocument& doc)
{
//	Serial.println("Call JSON Serial");
	if (doc.containsKey("UseMag"))
		magType = doc["UseMag"];
	if (doc.containsKey("UseIMU"))
		imuType = doc["UseIMU"];
	if (doc.containsKey("WheelDia"))
		wheelDia = doc["WheelDia"];
	if (doc.containsKey("ReverseDir"))
		reverseDir = doc["ReverseDir"];
	if (doc.containsKey("MountStyle"))
		mountStyle = doc["MountStyle"];
	if ((doc.containsKey("ScaleList")) && (doc.containsKey("ScaleIndex")))
	{
		uint8_t scaleIndex = doc["ScaleIndex"];
//		JsonArray ScaleArray = doc["ScaleList"];
//		JsonArray ScaleInfo = ScaleArray[scaleIndex];
		workData.modScale = doc["ScaleList"][scaleIndex]["Scale"];
//		Serial.println(workData.modScale);
		strcpy(workData.scaleName, doc["ScaleList"][scaleIndex]["Name"]);
//		Serial.println(doc["ScaleList"][scaleIndex]["Name"]);
	}
	else
	{
		workData.modScale = 87;
		strcpy(workData.scaleName, "HO");
	}
	if (doc.containsKey("Units"))
		workData.dispDim = (uint8_t)doc["Units"]; //0: metric; 1: imperial
	if (doc.containsKey("MagThreshold"))
		magThreshold = doc["MagThreshold"]; //0: metric; 1: imperial
	begin();
}
/*
void IoTT_TrainSensor::setTxCallback(txFct newCB)
{
	sensorCallback = newCB;
}
*/
void IoTT_TrainSensor::resetDistance()
{
//	Serial.println("resetDistance");
	if (xSemaphoreTake(sensorSemaphore, portMAX_DELAY) == pdPASS) 
	{
		workData.absIntegrator = 0;
		workData.relIntegrator = 0;
		dispData = workData;
		xSemaphoreGive(sensorSemaphore);
	}
}

void IoTT_TrainSensor::resetHeading()
{
//	Serial.println("resetHeading");
	if (xSemaphoreTake(sensorSemaphore, portMAX_DELAY) == pdPASS) 
	{
		reloadOffset = true;
		xSemaphoreGive(sensorSemaphore);
	}
}

float_t IoTT_TrainSensor::getPercOfAngle(float_t gForce)
{
	float_t radAngle = asin(gForce);
	return reverseDir ? (-100 * tan(radAngle)) : (100 * tan(radAngle));
}

sensorData IoTT_TrainSensor::getSensorData()
{
	if (xSemaphoreTake(sensorSemaphore, portMAX_DELAY) == pdPASS) 
	{
		sensorData cpyData = dispData;
		xSemaphoreGive(sensorSemaphore);
		if (!imuSensor)
		{
			cpyData.currRadiusTech = 0;
			for (uint8_t i = 0; i < 3; i++)
				cpyData.posVector_mm[i] = 0;
			for (uint8_t i = 0; i < 4; i++)
				cpyData.eulerVectorRad[i] = 0;
		}
		if (mountStyle == 0)
			M5.Imu.getAccel(&cpyData.imuVal[0], &cpyData.imuVal[1], &cpyData.imuVal[2]);
		else
			M5.Imu.getAccel(&cpyData.imuVal[2], &cpyData.imuVal[1], &cpyData.imuVal[0]);
		M5.Imu.getGyro(&cpyData.imuVal[3], &cpyData.imuVal[4], &cpyData.imuVal[5]);
		return cpyData;
	}
}

/*
 * Flat:  Left  Center  Right Forward
 * x		-1	0	1	
 * z		0	-1	0
 * 			Down	Flat	Up
 * y		-1		0		1
 * 
 * Upright Left  Center  Right Forward
 * x		0	1	0
 * z		-1	0	1

 * 			Down	Flat	Up
 * y		-1		0		1
 * 
 * Dir Chg * -1
*/

void IoTT_TrainSensor::setRepRate(AsyncWebSocketClient * newClient, int newRate)
{
//	Serial.println("setRepRate");
	refreshRate = newRate;
	lastWebRefresh = millis();
}

void IoTT_TrainSensor::reqDCCAddrWatch(AsyncWebSocketClient * newClient, int16_t dccAddr, bool simulOnly)
{
	Serial.printf("reqDCCAddrWatch %i\n", dccAddr);
	digitraxBuffer->awaitFocusSlot(dccAddr, simulOnly); //set DigitraxBuffers to watch for next speed or fct command and memorize loco

    waitForNewDCCAddr = true;
    String speedTableData;
    speedTableData.reserve(4096); // ??
	clrSpeedTable(speedTableData);
}

bool IoTT_TrainSensor::proceedToTrackEnd(bool origin)
{
//	if (speedSample.adminData.upDir == origin)
//		toggleDirCommand();
	return true;
}

bool IoTT_TrainSensor::reverseTestDir()
{
	return true;
}

void IoTT_TrainSensor::startTest(float_t trackLen, float_t vMax, uint8_t pMode)
{
	if (speedSample.adminData.speedTestRunning) return; //do not restart
	//verify that focusSlot is defined
	int8_t currSlot = digitraxBuffer->getFocusSlotNr();
	if ((currSlot > 0) || (digitraxBuffer->getLocoNetMode() == false))
	{
//		if (workData.dispDim == 1) //imperial
//			speedSample.adminData.testTrackLen = 25.4 * trackLen; //change inch to millimters
//		else
		speedSample.adminData.testTrackLen = trackLen;
		speedSample.adminData.sampleMinDistance = wheelDia * PI * minTestWheelTurns; //always mm
		speedSample.adminData.crawlSpeedMax = wheelDia * PI * crawlTurns; //always mm/s
//		Serial.printf("%.2f \n", speedSample.adminData.crawlSpeedMax);
		speedSample.adminData.testSteps = pMode == 0 ? 28 : 127;
//		Serial.printf("%i %i\n", speedSample.adminData.testSteps, pMode);
		speedSample.adminData.upDir = true;
		speedSample.adminData.testState[0].testPhase = 0; //start test
		speedSample.adminData.testState[1].testPhase = 0; 

        // Orginal creating a -1 self trigger state to force to forward
        // om start. This way after a stop there is no need top worry about
        // whether the engine is in forward or reverse,
        speedSample.adminData.masterPhase = -1; //run test
//		speedSample.adminData.masterPhase = 0; //run test
		speedSample.adminData.testState[0].lastSpeedStep = 0;
		speedSample.adminData.testState[1].lastSpeedStep = 0;
		speedSample.adminData.testState[0].crawlSpeedStep = 1;
		speedSample.adminData.testState[1].crawlSpeedStep = 1;
		speedSample.adminData.testState[0].vMaxComplete = false;
		speedSample.adminData.testState[1].vMaxComplete = false;
		speedSample.adminData.vMaxTest = vMax;
		//reset speed recording data

        String speedTableData;
        speedTableData.reserve(4096); // ??
		clrSpeedTable(speedTableData);
		//reset distance counter
		resetDistance();
		//start test process
		speedSample.adminData.speedTestTimer = millis();
		speedSample.adminData.currSpeedStep = 0;
		speedSample.adminData.validSample = false;
		speedSample.adminData.speedTestRunning = true;
        setRepRate(NULL, 5000);
	}
	else
		Serial.println("No focus slot assigned");
}

void IoTT_TrainSensor::sendSpeedCommand(uint8_t newSpeed)
{
	int8_t currSlot = digitraxBuffer->getFocusSlotNr();
	if (currSlot > 0 || (digitraxBuffer->getLocoNetMode() == false))
	{
		Serial.printf("Set Speed Slot %i to %i\n", currSlot, newSpeed);
		lnTransmitMsg txBuffer;
		txBuffer.lnData[0] = 0xA0; //OPC_LOCO_SPD 
		txBuffer.lnData[1] = currSlot;
		txBuffer.lnData[2] = map(newSpeed, 0, speedSample.adminData.testSteps, 0, maxSpeedSteps-1);
		txBuffer.lnMsgSize = 4;
		setXORByte(&txBuffer.lnData[0]);
		sendMsg(txBuffer);
	}
}

void IoTT_TrainSensor::forwardDirCommand()
{
	int8_t currSlot = digitraxBuffer->getFocusSlotNr();
	if ((currSlot > 0) || (digitraxBuffer->getLocoNetMode() == false))
	{
		slotData * focusSlot = digitraxBuffer->getSlotData(currSlot);
		lnTransmitMsg txBuffer;
		txBuffer.lnData[0] = 0xA1; //OPC_LOCO_DIRF 
		txBuffer.lnData[1] = currSlot;
		txBuffer.lnData[2] = ((*focusSlot)[3] | 0x20);
		Serial.printf("Slot %2X Sent %2X\n", (*focusSlot)[3], txBuffer.lnData[2]);
		txBuffer.lnMsgSize = 4;
		setXORByte(&txBuffer.lnData[0]);
		sendMsg(txBuffer);
		speedSample.adminData.upDir = true;
	}
}

void IoTT_TrainSensor::toggleDirCommand()
{
	int8_t currSlot = digitraxBuffer->getFocusSlotNr();
	if ((currSlot > 0) || (digitraxBuffer->getLocoNetMode() == false))
	{
		slotData * focusSlot = digitraxBuffer->getSlotData(currSlot);
		lnTransmitMsg txBuffer;
		txBuffer.lnData[0] = 0xA1; //OPC_LOCO_DIRF 
		txBuffer.lnData[1] = currSlot;
		txBuffer.lnData[2] = ((*focusSlot)[3] ^ 0x20);
		Serial.printf("Slot %2X Sent %2X\n", (*focusSlot)[3], txBuffer.lnData[2]);
		txBuffer.lnMsgSize = 4;
		setXORByte(&txBuffer.lnData[0]);
		sendMsg(txBuffer);
		speedSample.adminData.upDir = (!speedSample.adminData.upDir);
	}
}

void IoTT_TrainSensor::stopTest(String& speedTableData)
{
//	if (runSpeedTest)
	{
		//stop locomotive
        setRepRate(NULL, 0);
		sendSpeedCommand(0);
//		toggleDirCommand();
		speedSample.adminData.speedTestRunning = false;       
		sendSpeedTableDataToWeb(true, speedTableData);
	}
}

void IoTT_TrainSensor::clrSpeedTable(String& speedTableData)

{
//	Serial.println("Reset Table");
	for (uint8_t i = 0; i < maxSpeedSteps; i++)
	{
		speedSample.fw[i] = 0;
		speedSample.bw[i] = 0;
	}
	sendSpeedTableDataToWeb(false, speedTableData);
}

bool IoTT_TrainSensor::processTestStep(sensorData * sensStatus)
{
	uint32_t timeSince;
	bool hasProgress = (abs(sensStatus->relIntegrator - speedSample.adminData.testStartLinIntegrator) > 0) && (abs(sensStatus->relIntegrator - speedSample.adminData.lastLinIntegrator) > 0);
	speedSample.adminData.lastLinIntegrator = sensStatus->relIntegrator;
	if (speedSample.adminData.validSample)
	{
		timeSince = micros() - speedSample.adminData.measureStartTime;
        float distance = abs(sensStatus->relIntegrator - speedSample.adminData.testStartLinIntegrator);
        Log::print("Time since: ", LogLevel::INFO);
        Log::print(timeSince, LogLevel::INFO);
        Log::print(" Max: ", LogLevel::INFO);
        Log::print(maxSampleTime, LogLevel::INFO);
        Log::print(" Distance: ", LogLevel::INFO);
        Log::print(distance, LogLevel::INFO);
        Log::print(" Max: ", LogLevel::INFO);
        Log::println(speedSample.adminData.sampleMinDistance, LogLevel::INFO);

		if ((timeSince > (uint32_t)maxSampleTime) || (distance >= speedSample.adminData.sampleMinDistance))
		{
//			Serial.println("s cmpl");
			return true;
		}
	}
	else
	{
		timeSince = millis() - speedSample.adminData.testStartTime;

        Log::print("Time since: ", LogLevel::INFO);
        Log::print(timeSince, LogLevel::INFO);
        Log::print(" accel time: ", LogLevel::INFO);
        Log::println(speedSample.adminData.accelTime, LogLevel::INFO);
		if (timeSince > speedSample.adminData.accelTime) //(uint32_t)accel2Steps)
		{
//			Serial.println("s start");
			speedSample.adminData.testStartLinIntegrator = sensStatus->relIntegrator;
			speedSample.adminData.lastLinIntegrator = speedSample.adminData.testStartLinIntegrator;
			speedSample.adminData.measureStartTime = micros();
			speedSample.adminData.validSample = true;
            Log::println("Valid Sample!", LogLevel::INFO);
		}
	}
	return false;
}

bool IoTT_TrainSensor::processSpeedTest(String& speedTableData) //returns false if complete
{	
	//get distance data
 	int8_t currSlot = digitraxBuffer->getFocusSlotNr();
 	if ((currSlot > 0) || (digitraxBuffer->getLocoNetMode() == false))
 	{
 		slotData* focusSlot = digitraxBuffer->getSlotData(currSlot);

        // According to digitrax spec 1 is equal to forward?
        // D7-0 ;always 0
        // D6-SL_XCNT ; reserved , set 0
        // D5-SL_DIR ;1=loco direction FORWARD
        // D4-SL_F0 ;1=Directional lighting ON
        // D3-SL_F4 ;1=F4 ON
        // D2-SL_F3 ;1=F3 ON
        // D1-SL_F2 ;1=F2 ON
        // D0-SL_F1 ;1=F1 ON
        // original code?
        //	bool forwardDir =  ((*focusSlot)[3] & 0x20) == 0;  // syncs in -1 state
        // using this resolved fw bw mismatch...
        bool forwardDir =  ((*focusSlot)[3] & 0x20) == 0x20;
 		sensorData cpyData = getSensorData();
 		uint8_t upDirIndex = speedSample.adminData.upDir ? 1 : 0;
 		bool recordData = true;
		
        Log::print("Master phase = ", LogLevel::INFO);
        Log::println((int) speedSample.adminData.masterPhase, LogLevel::INFO);
 		switch (speedSample.adminData.masterPhase)
 		{
            case -1: // force to forward direction in case direction is reversed after test stop.
            {
                Log::println("Start test set forward.", LogLevel::INFO);
                forwardDirCommand();
                upDirIndex = speedSample.adminData.upDir ? 1 : 0;
                forwardDir = ((*focusSlot)[3] & 0x20) == 0x20;

               
                speedSample.adminData.testState[0].startSpeedStep = 0;
                speedSample.adminData.testState[1].startSpeedStep = 0;

                // Set these for testing to start at a different step.
                speedSample.adminData.testState[0].lastSpeedStep = 0;
                speedSample.adminData.testState[1].lastSpeedStep = 0;
                speedSample.adminData.testRemainingDistanceStartingLinIntegrator = 0;
                speedSample.adminData.masterPhase = 0;
                speedSample.adminData.result = 0;
            }
            break;
 			case 0: //proceed with directional test
 			{
 				//verify there is enough room more next step, otherwise change direction
                float_t distanceSoFarDirection = cpyData.relIntegrator - speedSample.adminData.testRemainingDistanceStartingLinIntegrator;
                // float_t remDist = speedSample.adminData.upDir ?
                //     speedSample.adminData.testTrackLen - abs(distanceSoFarStep) :
                //     abs(distanceSoFarStep);
                float_t remDist = speedSample.adminData.testTrackLen - abs(distanceSoFarDirection);

                // cap to zero.
                remDist = speedSample.adminData.upDir ?
                    (cpyData.relIntegrator < speedSample.adminData.testTrackLen ? remDist : 0) :
                    (cpyData.relIntegrator > 0 ? remDist : 0);

                // Original.
 				// float_t brakeDist = sq(cpyData.currSpeedTech) / 300;

                // making this shorter.
                float_t brakeDist = sq(cpyData.currSpeedTech) / 900;

                Log::print("Remainig Distance: ", LogLevel::INFO);
                Log::print(remDist, LogLevel::INFO);
                Log::print("  Brake Distance: ", LogLevel::INFO);
                Log::println(brakeDist, LogLevel::INFO);
 				if (remDist < brakeDist)
 				{
 					speedSample.adminData.masterPhase = 1;
 					speedSample.adminData.testState[upDirIndex].testPhase = 0; //prepare for reentry
 					return true;
 				}


 				//if there's room, proceed
                Log::print("Test phase = ", LogLevel::INFO);
                Log::print((uint8_t) speedSample.adminData.testState[upDirIndex].testPhase, LogLevel::INFO);
                Log::print("  upDirIndex = ", LogLevel::INFO);
                Log::println((uint8_t) upDirIndex, LogLevel::INFO);

 				switch (speedSample.adminData.testState[upDirIndex].testPhase)
 				{
 					case 0: //set initial speed
 					{
 						Log::print("Start Meas ", LogLevel::INFO);
                        Log::println((uint8_t) speedSample.adminData.testState[upDirIndex].lastSpeedStep, LogLevel::INFO);
 						uint8_t oldSpeed = speedSample.adminData.currSpeedStep; 

 						if (speedSample.adminData.testState[upDirIndex].lastSpeedStep == 0)
 						{
 							speedSample.adminData.currSpeedStep = speedSample.adminData.testSteps > 28 ? 2 : 1;
 							speedSample.adminData.testState[upDirIndex].testPhase++; //this is a valid test, so set testPhase to 2 be incrementing twice
 						}
 						else
 							speedSample.adminData.currSpeedStep = speedSample.adminData.testState[upDirIndex].lastSpeedStep;

                        // State 4 nd 5 replace this.
 						// speedSample.adminData.accelTime = (uint32_t)accel2Steps + (40 * (speedSample.adminData.currSpeedStep - oldSpeed));
                        speedSample.adminData.accelTime = (uint32_t) accel2Steps;
 						speedSample.adminData.validSample = false;
 						sendSpeedCommand(speedSample.adminData.currSpeedStep); //get started with speed step
 						speedSample.adminData.testStartTime = millis();
 						speedSample.adminData.testState[upDirIndex].testPhase++;
 					}
 					break;
 					case 1: //intermediate step for reentering routing, cancels recording of speed in the next step
 						recordData = false; 
 					//no break, just proceed
 					case 2: //increase speed and measure up to about 30mm/s, then determine direction and speed step for test direction, then move on
 						if (processTestStep(&cpyData))
 						{
 							Log::print("Dist: ", LogLevel::INFO);
                            Log::println(cpyData.relIntegrator, LogLevel::INFO);

 							uint32_t timeSince = micros() - speedSample.adminData.measureStartTime;
 							float_t distSince = cpyData.relIntegrator - speedSample.adminData.testStartLinIntegrator;
 							if ((speedSample.adminData.upDirPos == -1) && (abs(distSince) > 5))
 							{
 								speedSample.adminData.upDirPos = (distSince > 0) ^ (!speedSample.adminData.upDir);

                                Log::print("UpDirPost: ", LogLevel::INFO);
                                Log::println(speedSample.adminData.upDirPos, LogLevel::INFO);
 							}
 							float_t* dataEntry = forwardDir ? &speedSample.fw[speedSample.adminData.currSpeedStep] : &speedSample.bw[speedSample.adminData.currSpeedStep];
 							(*dataEntry) = abs(1000000 * distSince / timeSince);

                            if(speedSample.adminData.currSpeedStep < 126)
                            {
                                for(int i = speedSample.adminData.currSpeedStep + 1; i < 128; i++)
                                {
                                    if(forwardDir)
                                    {
                                        speedSample.fw[i] = (*dataEntry);
                                    }
                                    else
                                    {
                                        speedSample.bw[i] = (*dataEntry);
                                    }
                                }
                            }
							
 							Serial.printf("spd: %.2f %.2f %i %i\n", (*dataEntry), speedSample.adminData.crawlSpeedMax, speedSample.adminData.testState[upDirIndex].crawlSpeedStep, speedSample.adminData.currSpeedStep);
 							if (((*dataEntry) < speedSample.adminData.crawlSpeedMax) && (speedSample.adminData.testState[upDirIndex].crawlSpeedStep < speedSample.adminData.currSpeedStep))
 							{
 								speedSample.adminData.testState[upDirIndex].crawlSpeedStep = speedSample.adminData.currSpeedStep;
 								Log::print("set crawl speed ", LogLevel::INFO);
                                Log::println((uint8_t) speedSample.adminData.currSpeedStep, LogLevel::INFO);
 							}
 							speedSample.adminData.testState[upDirIndex].lastSpeedStep = speedSample.adminData.currSpeedStep;

 //							if ((speedSample.adminData.currSpeedStep < maxSpeedSteps) && ((*dataEntry) < (1.1 * speedSample.adminData.vMaxTest)))
 							if ((speedSample.adminData.currSpeedStep < speedSample.adminData.testSteps) &&
                                ((*dataEntry) < (1.1 * speedSample.adminData.vMaxTest)))
 							{                                
 								speedSample.adminData.currSpeedStep++;
 								speedSample.adminData.validSample = false;
 								sendSpeedCommand(speedSample.adminData.currSpeedStep); //set speed step
 								speedSample.adminData.testStartTime = millis();
 							}
 							else
 							{
 								speedSample.adminData.testState[upDirIndex].vMaxComplete = true;
 								speedSample.adminData.testState[upDirIndex].testPhase = 0;
 								speedSample.adminData.masterPhase = 1;
 							}
 							sendSpeedTableDataToWeb(false, speedTableData);

 //							Serial.println(speedSample.adminData.testState[upDirIndex].testPhase);
 						}
 					break;
 				}
 				return true;
 			}
 			break;
 			case 1: //slow move to track end
 			{
 				uint8_t newSpeed = 0;
 				float_t distFromOrigin = speedSample.adminData.upDir ? speedSample.adminData.testTrackLen - abs(cpyData.relIntegrator) : speedSample.adminData.upDirPos == 1 ? cpyData.relIntegrator : -cpyData.relIntegrator; //negative if overshooting
 //				Serial.printf("Dist: %0.2f %i %i \n", distFromOrigin, speedSample.adminData.upDir, speedSample.adminData.upDirPos);
 				if (distFromOrigin < 0)
 				{
 					newSpeed = 0;
 				}
 				else
 					newSpeed = speedSample.adminData.testState[upDirIndex].crawlSpeedStep;

                Log::print("Move to track end ", LogLevel::INFO);
                Log::print((uint8_t)newSpeed, LogLevel::INFO);
                Log::print(" to go: ", LogLevel::INFO);
                Log::println(distFromOrigin, LogLevel::INFO);
 				
				speedSample.adminData.currSpeedStep = newSpeed;
				sendSpeedCommand(speedSample.adminData.currSpeedStep); //set speed step

 				if (newSpeed == 0)
 					speedSample.adminData.masterPhase++;
 			}
 			break;
            case 2: // wait for stop
                if(abs(cpyData.currSpeedTech) > 0.5)
                {

                    // wait for decel.
                    Log::print("Wait for stop: [mm/s] ", LogLevel::INFO);
                    Log::println(cpyData.currSpeedTech, LogLevel::INFO);
                    return true;
                }
                speedSample.adminData.testRemainingDistanceStartingLinIntegrator = cpyData.relIntegrator;
                speedSample.adminData.masterPhase++;
            break;
 			case 3: //slow down and toggle direction
 				toggleDirCommand();
 				if (speedSample.adminData.upDir && speedSample.adminData.testState[0].vMaxComplete && speedSample.adminData.testState[1].vMaxComplete)
                {

                    // Normal finish.
                    Log::print("Test completed successfully.", LogLevel::INFO);
                    speedSample.adminData.result = 1;
 					return false;
                }
                if ((speedSample.adminData.upDir && speedSample.adminData.testState[0].vMaxComplete || speedSample.adminData.testState[1].vMaxComplete) &&
                    (speedSample.adminData.testState[0].lastSpeedStep > 126 && speedSample.adminData.testState[1].lastSpeedStep > 126 ))
                {

                    // Warning only one direction was able to reach vMax.
                    Log::print("Only one side completed v Max.", LogLevel::INFO);
                    speedSample.adminData.result = 2;
 					return false;
                }
                if(((speedSample.adminData.testState[upDirIndex].lastSpeedStep - speedSample.adminData.testState[upDirIndex].startSpeedStep) < 2) &&
                    (speedSample.adminData.testState[0].lastSpeedStep < 126 && speedSample.adminData.testState[1].lastSpeedStep < 126 ))
                {

                    // Not enough track length for speed step measurement.
                    // Re-run test with longer track length.
                    Log::print("Tracklength too short.", LogLevel::INFO);
                    speedSample.adminData.result = 3;
                    return false;
                }
 				speedSample.adminData.testStartTime = millis();

                // Set speed to previous step 4 waits for accel. 
                sendSpeedCommand(speedSample.adminData.testState[!upDirIndex].lastSpeedStep);
                speedSample.adminData.testState[upDirIndex].startSpeedStep = speedSample.adminData.testState[upDirIndex].lastSpeedStep;
 				speedSample.adminData.masterPhase++;
 			break;
            case 4:
                if(speedSample.adminData.testState[upDirIndex].lastSpeedStep > 1)
                {
                    float_t dataEntry = forwardDir ? speedSample.fw[speedSample.adminData.testState[upDirIndex].lastSpeedStep] : speedSample.bw[speedSample.adminData.testState[upDirIndex].lastSpeedStep];
                    float_t distanceSoFarDirection = cpyData.relIntegrator - speedSample.adminData.testRemainingDistanceStartingLinIntegrator;

                    if(abs(cpyData.currSpeedTech) < (0.80 * dataEntry) &&
                        (abs(distanceSoFarDirection) < (0.35 * speedSample.adminData.testTrackLen)))
                    {
                        Log::print("Wait for speed: [mm/s] ", LogLevel::INFO);
                        Log::print(cpyData.currSpeedTech, LogLevel::INFO);
                        Log::print("accel: [km/h s]", LogLevel::INFO);                        
                        Log::println(cpyData.currScaleAccelTech, LogLevel::INFO);                       
                        Log::print("distance so far: ", LogLevel::INFO);
                        Log::println(distanceSoFarDirection , LogLevel::INFO);
                        return true;
                    }                   
                }
                speedSample.adminData.masterPhase++;
            break;
 			case 5:
 				if ((millis() - speedSample.adminData.testStartTime) > 1000)
 					speedSample.adminData.masterPhase = 0; //back to testing
 				break;
 		}
 		return true;
 	}
	return true;
}

void IoTT_TrainSensor::programmerReturn(uint8_t * programmerSlot)
{
	uint16_t opsAddr = (programmerSlot[2]<<7) + (programmerSlot[3] & 0x7F);
	uint16_t cvNr = ((programmerSlot[5] & 0x30)<<4) + ((programmerSlot[5] & 0x01)<<7) + (programmerSlot[6] & 0x7F) + 1;
	uint8_t cvVal = (programmerSlot[7] & 0x7F) + ((programmerSlot[5] & 0x02)<<6);
//	Serial.printf("Prog Stat: %i ps: %i CV: %i Val: %i\n", programmerSlot[1], opsAddr, cvNr, cvVal);
	int8_t currClient = getWSClientByPage(0, "pgPrplHatCfg");
	if (currClient >= 0)
	{
		DynamicJsonDocument doc(200);
		char myMqttMsg[200];
		doc["Cmd"] = "ProgReturn";
		JsonObject Data = doc.createNestedObject("Data");
		Data["Status"] = programmerSlot[1];
		Data["OpsAddr"] = opsAddr;
		Data["CVNr"] = cvNr;
		Data["CVVal"]= cvVal;
		serializeJson(doc, myMqttMsg);
//		Serial.println(myMqttMsg);
		while (currClient >= 0)
		{
			globalClients[currClient].wsClient->text(myMqttMsg);
			currClient = getWSClientByPage(currClient + 1, "pgPrplHatCfg");
		}
	}
	else
		Serial.println("No global");
}

void IoTT_TrainSensor::sendSpeedTableDataToWeb(bool isFinal, String& speedTableData)
{ 	
    DynamicJsonDocument doc(5120);
 	doc["Cmd"] = "SpeedTableData";

 	JsonObject Data = doc.createNestedObject("Data");
 	Data["SlotNr"] = digitraxBuffer->getFocusSlotNr();
 	Data["NumSteps"] = speedSample.adminData.testSteps;
    Data["Result"] = speedSample.adminData.result;
 	
    JsonArray fwArray = Data.createNestedArray("fw");
 	JsonArray bwArray = Data.createNestedArray("bw");
 	
    if (isFinal)
    {
 		Data["final"] = true;
    }

 	for (int i = 0; i <= speedSample.adminData.testSteps; i++)
 	{
 		fwArray.add(speedSample.fw[i]);
 		bwArray.add(speedSample.bw[i]);
 	}
 	serializeJson(doc, speedTableData);
    Log::print("Test result: ", LogLevel::DEBUG);
 	Log::println(speedTableData.c_str(), LogLevel::DEBUG);
}	

void IoTT_TrainSensor::sendSensorDataToWeb(String& data)
{	
	sensorData cpyData = getSensorData();
	DynamicJsonDocument doc(512);	

	doc["Cmd"] = "SensorData";
	JsonObject Data = doc.createNestedObject("Data");
	Data["Speed"] = cpyData.currSpeedTech;
	Data["AbsDist"] = cpyData.absIntegrator;
	Data["RelDist"] = cpyData.relIntegrator;
	Data["AxisAngle"] = cpyData.axisAngle;
	int8_t dirFlag = cpyData.currSpeedTech >= 0 ? 1 : -1;
	Data["Slope"] = dirFlag * getPercOfAngle(cpyData.imuVal[1]);
	Data["Banking"] = dirFlag * getPercOfAngle(cpyData.imuVal[0]);
    Data["SamplingRate"] = cpyData.avgTimeInterval;
    Data["ScaleSpeed"] = cpyData.currScaleSpeedTech;
    Data["Accel"] = cpyData.currScaleAccelTech;  

//	Serial.println(cpyData.axisAngle);

//		if (imuSensor)
	{
		Data["Radius"] = cpyData.currRadiusTech;
		for (uint8_t i = 0; i < 3; i++)
			Data["PosVect"][i] = cpyData.posVector_mm[i];
		for (uint8_t i = 0; i < 4; i++)
			Data["EulerVect"][i] = cpyData.eulerVectorRad[i];
	}
	Data["TS"] = cpyData.timeStamp;
	int8_t currSlotNr = digitraxBuffer->getFocusSlotNr();
	if (currSlotNr >= 0)
	{
	    slotData* currSlot = digitraxBuffer->getSlotData(currSlotNr);
	 	if (currSlot)
	 	{
	 		uint16_t thisAddr = (((*currSlot)[6] & 0x7F) << 7) + ((*currSlot)[1] & 0x7F); //from IoTT_DigitraxBuffers.h
	 		Serial.printf("Slot: %i Addr %i\n", currSlotNr, thisAddr);
	 		if (thisAddr > 0)
	 		{
	 			Data["DCCAddr"] = thisAddr;
	 			Data["SpeedStep"] = (*currSlot)[2]; //current speed step
	 			Data["DirF"] = (*currSlot)[3]; //current DIRF
	 			waitForNewDCCAddr = false;
	 		}
	 	}
	}

	serializeJson(doc, data);
//	Serial.println(myMqttMsg);
	// while (currClient >= 0)
	// {
	// 	globalClients[currClient].wsClient->text(myMqttMsg);
	// 	currClient = getWSClientByPage(currClient + 1, "pgPrplHatCfg");
	// }
	lastWebRefresh += refreshRate;	
}

void IoTT_TrainSensor::processSensorLoop(String& sensorData)
{
	delay(2);
	if (refreshRate > 0)
	{
		if ((millis() - refreshRate) > lastWebRefresh)
		{
			sendSensorDataToWeb(sensorData);
		}
	}
	else
		if (waitForNewDCCAddr)
		{
			int8_t currSlotNr = digitraxBuffer->getFocusSlotNr();
			if (currSlotNr >= 0)
			{
				slotData * currSlot = digitraxBuffer->getSlotData(currSlotNr);
				if (currSlot)
				{
					uint16_t thisAddr = (((*currSlot)[6] & 0x7F) << 7) + ((*currSlot)[1] & 0x7F); //from IoTT_DigitraxBuffers.h
					if (thisAddr > 0)
					{
						Serial.printf("Slot: %i Addr %i\n", currSlotNr, thisAddr);
						sendSensorDataToWeb(sensorData);
			 		}
			 	}
			}
		}
}

void IoTT_TrainSensor::processSpeedTestLoop(String& speedTestData)
{	
	if (speedSample.adminData.speedTestRunning)
    {
		if (millis() - speedSample.adminData.speedTestTimer > speedTestInterval)
		{
			Serial.printf("Test %i\n", 1);
			speedSample.adminData.speedTestRunning = processSpeedTest(speedTestData);
			if (!speedSample.adminData.speedTestRunning)
			{
				//process data and send to web client
				Serial.println("Test complete");
				speedSample.adminData.validSample = false;
				stopTest(speedTestData);
			}
			speedSample.adminData.speedTestTimer += speedTestInterval;
		}
    }
}
