# PurpleHatModule
Purple hat is a hardware platform by Hans Tanner used to automate speed profiling for Digital Command
Control (DCC) equipped model railroad locomotives.  Both the hardware design and provided software are
open source and can by found on the IOTT stick git hub repository:

https://github.com/tanner87661/IoTTStick

In addition many excellent support videos can be found on the IOTT you tube channel:

https://www.youtube.com/c/IoTTCloud

Here are just a few that highlight the development of the purple hat, but check out the channel for these
and many more concerning all apsects of using new technology with the model railroading hobby.

| | | |
|---|---|---|
| <a href="http://www.youtube.com/watch?feature=player_embedded&v=xracbWHlD6M&t=239s" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/ec230ff4-95a0-45e8-ab12-ac9d04c1f59f" alt="Layout GPS (Video#82)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/01YRP97fmiU?si=FaMYlQxXjFY6Y-N6" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/31066416-17c1-4765-8b51-a5a25231c5cd" alt="DIY Track Measuring Car for HO and N Scale (Video#87)" width="320" height="180" border="10" /></a> | <a href="http://www.youtube.com/watch?feature=player_embedded&v=YqyiDbs_tls" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/f9342e0e-6e41-4b3d-84a3-e844e1e330d2" alt="DIY Track Measuring Car Test (Video#88)" width="320" height="180" border="10" /></a> |
| <a href="https://youtu.be/0ds42XWqe9w?si=bT1opp3O2ZBgWA5c" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/fc8ecb74-a204-49ec-ac95-809a2d04ca8c" alt="Speed Matching Re-Invented (Video#90)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/u_VjYLT28Q0?si=-LNIpt8-L52iYIAo" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/efd4f8da-cbba-4f6c-93e0-6cd78761512a" alt="One Minute Speed Profiling (Video#91)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/XExpASPmUMI?si=slxODqN88dGGZjpG" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/e0baeb8c-95be-4649-82d3-58d21e9a2933" alt="Build your own PurpleHat Sensor (Video#93)" width="320" height="180" border="10" /></a>  |
| <a href="http://www.youtube.com/watch?feature=player_embedded&-WRvbC5DXH4&t=1095s" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/2c2a8e60-8bcd-4e9d-89ae-403a8159313f" alt="Speed Profiling Step by Step Tutorial (Video#94)" width="320" height="180" border="10" /></a> | <a href="http://www.youtube.com/watch?feature=player_embedded&b0Au8o2HQeM&t=313s" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/b8fb7a6f-96c6-4692-b217-a915872edf3a" alt="PurpleHat Tips, Tricks, and Apps (Video#96)" width="320" height="180" border="10" /></a> | <a href="http://www.youtube.com/watch?feature=player_embedded&pA4SSrq9UFE" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/4f079739-6052-4d88-bcbb-1622c72744a4" alt="Speed Profiling Station with Arduino / DCC EX (Video#116)" width="320" height="180" border="10" /></a> |
| <a href="http://www.youtube.com/watch?feature=player_embedded&KTQQpCsVA8E&t=46s" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/d8625f87-196a-451a-9b2c-4198fdf44754" alt="PurpleHat Tips, Tricks and new Features (Video#122)" width="320" height="180" border="10" /></a> | <a href="https://youtu.be/s9Wfw1m8l4A?si=cJCRfqefYz01u9v1" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/41a891ef-07f1-4ca6-88f5-3d626e38f1e2" alt="Closing the Loop - More New PurpleHat Features (Video#123)" width="320" height="180" border="10" /> | <a href="https://youtu.be/hhmCFm9BiHA?si=9rewAfW110-CZBXg" target="_blank"><img src="https://github.com/AlgerP572/PurpleHatModule/assets/13104848/1bad9f66-5e39-4925-a38d-a055d42cf9f7" alt="Make it tiny: PurpleHat Mini (Video#131)" width="320" height="180" border="10" /> |

This github repository comprises a derivative work from Hans Tanner's Purple hat IOTT stick SW.  The
derivative software supports over the air (OTA) firmware (FW) download and OTA logging. Adding the
ability to download FW over the air wirelessely allows the M5 stick to be updated in place on the layout
without a COM port connection.  This is a nice feature since the battery I use to run the M5 stick does
not allow for COM port access. With OTS FW dowload I can also update the M5 FW without having to remove
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


# Measurements Folder
This folder contains various measurements I've made for the G-scale engines in the fleet.  The roster folder contains an excel template that documents the various steps for the profiling. i.e. Set high, mid, low speed control, CV29 bit 4, wheel diameter etc.  It also shows an overview of the pre-calibrated, speed table and post calibrated data. An example for the RhB 413 Krocodile is below:

![image](https://github.com/AlgerP572/PurpleHatModule/assets/13104848/8b91f67d-3675-4337-b381-6c1d26ef4cdb)

