# PurpleHat- Background
Purple hat is a hardware platform by Hans Tanner used to automate speed profiling for Digital Command
Control (DCC) equipped model railroad locomotives.  Both the hardware design and provided software are
open source and can by found on the internet of toy trains (IOTT) stick git hub repository:

https://github.com/tanner87661/IoTTStick

In addition many excellent support videos can be found on the IOTT you tube channel:

https://www.youtube.com/c/IoTTCloud

Here are just a few that highlight the development of the purple hat, but check out the channel for these
and many more concerning all apsects of using new technology with the model railroading hobby.

| | | |
|---|---|---|
| <a href="http://www.youtube.com/watch?feature=player_embedded&v=xracbWHlD6M&t=239s" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/ec230ff4-95a0-45e8-ab12-ac9d04c1f59f" alt="Layout GPS (Video#82)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/01YRP97fmiU?si=FaMYlQxXjFY6Y-N6" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/31066416-17c1-4765-8b51-a5a25231c5cd" alt="DIY Track Measuring Car for HO and N Scale (Video#87)" width="320" height="180" border="10" /></a> | <a href="http://www.youtube.com/watch?feature=player_embedded&v=YqyiDbs_tls" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/f9342e0e-6e41-4b3d-84a3-e844e1e330d2" alt="DIY Track Measuring Car Test (Video#88)" width="320" height="180" border="10" /></a> |
| <a href="https://youtu.be/0ds42XWqe9w?si=bT1opp3O2ZBgWA5c" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/fc8ecb74-a204-49ec-ac95-809a2d04ca8c" alt="Speed Matching Re-Invented (Video#90)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/u_VjYLT28Q0?si=-LNIpt8-L52iYIAo" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/efd4f8da-cbba-4f6c-93e0-6cd78761512a" alt="One Minute Speed Profiling (Video#91)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/XExpASPmUMI?si=slxODqN88dGGZjpG" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/e0baeb8c-95be-4649-82d3-58d21e9a2933" alt="Build your own PurpleHat Sensor (Video#93)" width="320" height="180" border="10" /></a>  |
| <a href="https://youtu.be/-WRvbC5DXH4?si=BaNzjVfViuEvEctT" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/2c2a8e60-8bcd-4e9d-89ae-403a8159313f" alt="Speed Profiling Step by Step Tutorial (Video#94)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/pA4SSrq9UFE?si=hkQaUZnqgn0DI3pb" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/b8fb7a6f-96c6-4692-b217-a915872edf3a" alt="PurpleHat Tips, Tricks, and Apps (Video#96)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/b0Au8o2HQeM?si=5GhcrTF3LrSURjIQ" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/4f079739-6052-4d88-bcbb-1622c72744a4" alt="Speed Profiling Station with Arduino / DCC EX (Video#116)" width="320" height="180" border="10" /></a> |
| <a href="https://youtu.be/KTQQpCsVA8E?si=uP8bcGdStDltAaaL" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/d8625f87-196a-451a-9b2c-4198fdf44754" alt="PurpleHat Tips, Tricks and new Features (Video#122)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/s9Wfw1m8l4A?si=cJCRfqefYz01u9v1" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/41a891ef-07f1-4ca6-88f5-3d626e38f1e2" alt="Closing the Loop - More New PurpleHat Features (Video#123)" width="320" height="180" border="10" /> | <a href="https://youtu.be/hhmCFm9BiHA?si=9rewAfW110-CZBXg" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/1bad9f66-5e39-4925-a38d-a055d42cf9f7" alt="Make it tiny: PurpleHat Mini (Video#131)" width="320" height="180" border="10" /> |

# PurpleHatModule
This github repository comprises a derivative work from Hans Tanner's Purple hat IOTT stick SW.  The
derivative software supports over the air (OTA) firmware (FW) download and OTA logging. Adding the
ability to download FW over the air wirelessely allows the M5 stick to be updated in place on the layout
without a COM port connection.  This is a nice feature since the battery I use to run the M5 stick does
not allow for COM port access. With OTA FW dowload I can also update the M5 FW without having to remove
the M5 from the battery.

![image](https://github.com/AlgerP572/PurpleHatModule/assets/13104848/e121b220-d1f1-422f-9402-e60637b5293a)

```
Note: This is Purple hat only other hats have been removed and only a subset of the features are supprted.
This was to make flash memory space available for the the over the air FW download and over the air debug
logging.
```

# Building / Compiling PurpleHatModule
The purple hat module code has been converted to a platform.io project hosted by microsoft visual studio code.
Both the IDE and platformIO are free to use.  The main motivation for this change was to support incremental
builds which lead to build / compile times on average of less than 40 seconds.  The repo has the required
launch file in the .vscode folder in the src tree.

![image](https://github.com/AlgerP572/PurpleHatModule/assets/13104848/58a90ea3-a99b-4240-99ad-5c8c3da7cb37)

The required platformio.ini file setup to build for the M5-stick is also provided in the main src folder.  This
links to all of the external libraries as well as specifies the spiffs settings for the memory layout.  The
min-spiffs.csv is also provided in the main src folder.

```
; PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env]
platform = espressif32
framework = arduino
board_build.partitions = min_spiffs.csv
lib_deps =  https://github.com/m5stack/M5Unified.git                                  
            https://github.com/me-no-dev/AsyncTCP.git
            https://github.com/alanswx/ESPAsyncWiFiManager.git
            https://github.com/me-no-dev/ESPAsyncWebServer.git
            https://github.com/ayushsharma82/AsyncElegantOTA.git
            https://github.com/ayushsharma82/WebSerial.git           
            https://github.com/bblanchon/ArduinoJson.git
            https://github.com/adafruit/Adafruit_Sensor.git
            https://github.com/adafruit/Adafruit_BNO055.git
            https://github.com/taranais/NTPClient.git

[env:m5stick-c]
board = m5stick-c
monitor_speed = 115200
upload_speed = 115200
build_type = debug
monitor_filters = esp32_exception_decoder
```
# Added Features
The purple hat module SW introduces several additional features not present in the original purple
hat IOTT version.  These features are designed with G-scale modellers in mind, and help to ease the
process of the speed magic test. While tailored for G-scale, the new features may also prove valuable
for enthusiasts in other scales. If there is interest in these features, let me know and perhaps I
can ask Hans Tanner about including one or more of them into the main IOTT repository.

## Force Forward At Speed Magic Test Start.
The first feature is not scale-specific but rather pertains to the setup used to run purple hat and the IOTT
stick. I obvserved that, when aligning the engine to the test starting position from the reverse direction,
the engine would move backward instead of forward for the first leg of the speed magic test.  This
occasionally led to a direction mismatch, requiring a reboot of the IOTT stick.

To address this issue, a simple workaround is to stop the test, align the engine from the forward direction
or manually set forward direction from the throttle.  Many times I forgot to set forward direction and
the test wound up with the direction mismatch. As a solution, I introdcued a -1 state to the
PurpleHatModule state machine for the speed magic test. This new state automatically sets the direction of the
engine to forward, effectively resolving the problem.  This adjustment not only saves time but also
eliminates one more thing to remember during testing.

```
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
```

## Warning For Short Track Length
One of the more difficult settings to tune to the correct value was the track length for the test.  Through 
this process I discovered that the speed magic test perfoms best with G scale with long distances.  Currently
the shortest distance I use is 13500 mm.  Ocassionally, I found with distances that were too short that the
test was not able to measure a single speed step and it could get into an infinite loop since the step counter
was not effectively incrementing.  To fix this a feauture was added to the test where it now requires a minumum
of at least two speed steps to be completed in a single speed magic pass.  If the minimum two speed steps are
not completed in a given test test cycle, the stops width a warning that the track length may be too short.

![image](https://github.com/AlgerP572/PurpleHatModule/assets/13104848/e785ec78-c405-4abf-95d8-40059400ea7e)

## Support For Testing With Acceleration / Deceleration Values
The purple hat sensor, as provided from IOTT, requires setting the acceleration and decleration CVs for the decoder
to a value of 0.  Unfortuately, many G-scale can suffer from cracked drive gears if drive forces exceed
certain levels.  At higher speeds, I felt uneasy with the engines being directly driven to high 
speeds and stopped very quickly during the speed magic test.  As a consequence, in the purple hat module version
of the test, support was added to wait for the engine to come up to speed before the testing resumes for the
next test sequence. A downside to this approach is that the speed magic test will require a longer track length in
order to complete successfully.  See the previous feature regarding the warning added to support a track length that
is too short.  This can help "dial in" the correct track length for the speed magic test.

As with the force forward direction feature, an additional state was added to the state machine to achieve this new function.
(It is step 4 and the original IOTT step four is now step 5 in the state machine.)  As can be seen with this new step
in the state machine the speed magic test waits for either the speed to be 80% of the speed from where the test left
off or 35% of the tracklength distance used for the test (whichever comes first).  In this way smaller acceleration and
deceleration values can be used to reduce stress on engine drive components.

```
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
```

## Measured Profile To Speed Profile

It is still possible in G-scale to find engines whose decoders do not support the 28-step speed table of the NMRA
DCC standard.  However it may still be desirable to run a second engine in a consist with the engine that does not
support programming custom speed tables.  One option could be to replace the decoder in the engine that does not
support speed matching with one that does.  But the original decoder may have other desirable features such as
custom sounds that may not be available on the replacement decoder.

Purple Hat Module adds a feature where a measured locomotive speed profile can be converted to a throttle profile.
The new throttle profile can be used with IOTT pruple hat's speed table recalculation feature to generate a new
custom speed table for the other engine in the consist. Obivously any engine in the consist must be capable of having its
speed table programmed. Now both engines can be consisted together becuase the second engine has been speed matched
to the first engine that did not support 28-step speed tables.


# Measurements Folder
This folder contains various measurements I've made for the G-scale engines in the fleet.  The roster folder contains an excel template that documents the various steps for the profiling. i.e. Set high, mid, low speed control, CV29 bit 4, wheel diameter etc.  It also shows an overview of the pre-calibrated, speed table and post calibrated data. An example for the RhB 616 Kohle is below:

![image](https://github.com/AlgerP572/PurpleHatModule/assets/13104848/fc8fef9e-1425-40cb-8fe9-ff00b0df5935)


