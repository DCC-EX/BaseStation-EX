#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-02-11 15:02:18

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include "config.h"
#include "Queue.h"
void setupOTA() ;
void startOTA() ;
void onWSEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, 		AwsEventType type, void * arg, uint8_t *data, size_t len) ;
void handleNotFound(AsyncWebServerRequest *request) ;
void handleESPInfo(AsyncWebServerRequest *request) ;
void handleIndex(AsyncWebServerRequest *request) ;
void handleDCCppStatus(AsyncWebServerRequest *request) ;
void setup() ;
void loop() ;

#include "DCCppESP.ino"


#endif
