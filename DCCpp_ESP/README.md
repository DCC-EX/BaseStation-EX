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
* ArduinoJson (https://github.com/bblanchon/ArduinoJson)
* TaskScheduler (https://github.com/arkhipenko/TaskScheduler)

Additionally the Arduino IDE will require ESP8266 support, see https://github.com/esp8266/Arduino
for details in setting up the additional packages for this.

Uploading DCC++ ESP to your device
----------------------------------

The Arduino IDE or Arduino Eclipse IDE provide the necessary support for uploading to the ESP8266
device, there is no longer a SPIFFS binary to upload so no manual steps are required for that.
