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

The easiest way to build DCC++ ESP is to use Platform IO IDE, this will ensure the
necessary libraries are installed and everything is automated.

Building DCC++ ESP requires the following libraries:
* ArduinoOTA (https://github.com/esp8266/Arduino)
* ESP8266mDNS (https://github.com/esp8266/Arduino)
* ESP8266WiFi (https://github.com/esp8266/Arduino)
* ESPAsyncTCP (https://github.com/me-no-dev/ESPAsyncTCP)
* ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
* ArduinoJson (https://github.com/bblanchon/ArduinoJson)
* TaskScheduler (https://github.com/arkhipenko/TaskScheduler)

Additionally the Arduino IDE will require ESP8266 support, see https://github.com/esp8266/Arduino
for details in setting up the additional packages for this.

Uploading DCC++ ESP to your device
----------------------------------

The Arduino IDE provides the basic support for compiling and uploading the binary, but does not
provide the support for SPIFFS upload without adding additional plugins.

The easiest way to upload this to your ESP device is to use Platform IO IDE and execute the
following commands from a terminal window in the project directory:
platformio init
platformio run --target upload
platformio run --target uploadfs

This should compile and upload both the binary and SPIFFS data to the ESP device.
