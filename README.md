# PurpleHatModule
Purple hat is a hardware platform by Hans Tanner used to automate speed profiling for Digital Command
Control (DCC) equipped model railroad locomotives.  Both the hardware design and provided software are
open source and can by found on the IOTT stick git hub repository:

https://github.com/tanner87661/IoTTStick

In addition many excellent support videos can be found on the IOTT you tube channel:

https://www.youtube.com/c/IoTTCloud

<a href="http://www.youtube.com/watch?feature=player_embedded&v=xracbWHlD6M&t=239s" target="_blank"><img src="http://i.ytimg.com/vi/xracbWHlD6M&t=239s/0.jpg" alt="[Layout GPS (Video#82)" width="240" height="180" border="10" /></a>

<a href="http://www.youtube.com/watch?feature=player_embedded&v=YqyiDbs_tls" target="_blank"><img src="http://i.ytimg.com/vi/YqyiDbs_tls/0.jpg" alt="DIY Track Measuring Car Test (Video#88)" width="240" height="180" border="10" /></a>


| | |
|---|---|
| [!](https://img.youtube.com/vi/xracbWHlD6M&t=239s/1.jpg)](https://www.youtube.com/watch?v=xracbWHlD6M&t=239s) | [![](https://img.youtube.com/vi/YqyiDbs_tls/1.jpg)](https://www.youtube.com/watch?v=YqyiDbs_tls) |
| [![Speed Matching Re-Invented (Video#90)](https://img.youtube.com/vi/0ds42XWqe9w&t=463s/1.jpg)](https://www.youtube.com/watch?v=0ds42XWqe9w&t=463s) | [![One Minute Speed Profiling (Video#91)](https://img.youtube.com/vi/u_VjYLT28Q0&t=207s/1.jpg)](https://www.youtube.com/watch?v=u_VjYLT28Q0&t=207s) |
| [![Build your own PurpleHat Sensor (Video#93)](https://img.youtube.com/vi/XExpASPmUMI/1.jpg)](https://www.youtube.com/watch?v=XExpASPmUMI) | [![Speed Profiling Step by Step Tutorial (Video#94)](https://img.youtube.com/vi/-WRvbC5DXH4&t=1095s/1.jpg)](https://www.youtube.com/watch?v=-WRvbC5DXH4&t=1095s) |
| [![Speed Profiling Station with Arduino / DCC EX (Video#116)](https://img.youtube.com/vi/pA4SSrq9UFE/1.jpg)](https://www.youtube.com/watch?v=pA4SSrq9UFE) | [![PurpleHat Tips, Tricks, and Apps (Video#96)](https://img.youtube.com/vi/b0Au8o2HQeM&t=313s/1.jpg)](https://www.youtube.com/watch?v=b0Au8o2HQeM&t=313s) |
| [![PurpleHat Tips, Tricks and new Features (Video#122)](https://img.youtube.com/vi/KTQQpCsVA8E&t=46s/1.jpg)](https://www.youtube.com/watch?v=KTQQpCsVA8E&t=46s) | |


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

