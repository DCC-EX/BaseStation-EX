What is DCC++ ESP
-----------------

DCC++ ESP is an addon to the DCC++ BaseStation that provides a WiFi <-> Serial bridge with extras.
The extra features that DCC++ ESP provides are:
* Port 2560 listener for JMRI connections
* http server providing a web based two cab throttle
* web socket listener (used by web based throttle)

Configuring DCC++ ESP
---------------------

There isn't much to configure within DCC++ ESP itself.  config.h provides default values that match
what is provided in DCC++.

Building DCC++ ESP
------------------

Building DCC++ ESP requires the following libraries:
* ArduinoOTA (https://github.com/esp8266/Arduino)
* ESP8266mDNS (https://github.com/esp8266/Arduino)
* ESP8266WiFi (https://github.com/esp8266/Arduino)
* ESPAsyncTCP (https://github.com/me-no-dev/ESPAsyncTCP)
* ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)

Additionally the Arduino IDE will require ESP8266 support, see https://github.com/esp8266/Arduino
for details in setting up the additional packages for this. Arduino Eclipse can be used as well and
provides a board definition for the ESP8266.  For Arduino IDE the SPIFFS upload plugin is also
required: https://github.com/esp8266/arduino-esp8266fs-plugin. Arduino Eclipse does not yet support
automated SPIFFS upload.

When setting up the project in the IDE be sure to select the appropriate ESP device and that you
select the 128kb SPIFFS setting.

Uploading DCC++ ESP to your device
----------------------------------

The IDE should provide upload support by default. If you are using Arduino IDE you must also upload
the SPIFFS piece via Tools -> ESP8266 Sketch Data Upload.  For Arduino Eclipse it must be done
manually:
* mkspiffs.exe -c data -p 256 -b 4096 -s 131072 spiffs.bin
* esptool.exe -cd none -cb 115200 -cp COM3 -ca 0xDB000 -cf spiffs.bin

These values are based on a 512Mbit+512MBit ESP module.  Other modules may require a different base
address for upload.