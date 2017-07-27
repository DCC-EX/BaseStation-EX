/**********************************************************************
DCCppESP.ino
COPYRIGHT (c) 2017 Mike Dunston
Part of DCC++ BASE STATION for the Arduino / ESP8266
**********************************************************************/

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include "config.h"
#include "Queue.h"

#define UNUSED(x) (void)(x)

WiFiServer DCCppServer(DCCPP_CLIENT_PORT);
WiFiClient DCCppClients[MAX_DCCPP_CLIENTS];
AsyncWebServer webServer(80);
AsyncWebSocket webSocket("/ws");

Queue<String> DCCppPendingCommands(64);
Queue<String> DCCppOutboundPending(32);
String currentDCCppCommand = "";

bool OTARunning = false;
bool inDCCPPCommand = false;

void setupOTA() {
	ArduinoOTA.setPort(OTA_PORT);
	if (strlen(OTA_HOSTNAME) > 0) {
		ArduinoOTA.setHostname(OTA_HOSTNAME);
	}
	if (strlen(OTA_PASSWORD) > 0) {
		ArduinoOTA.setPassword(OTA_PASSWORD);
	}
	ArduinoOTA.onStart([]() {
		Serial.println("OTA: Start");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nOTA: End");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("OTA: Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("OTA: Error[%u]: ", error);
		switch(error) {
			case OTA_AUTH_ERROR:
			Serial.println("OTA: Auth Failed");
			break;
			case OTA_BEGIN_ERROR:
			Serial.println("OTA: Begin Failed");
			break;
			case OTA_CONNECT_ERROR:
			Serial.println("OTA: Connect Failed");
			break;
			case OTA_RECEIVE_ERROR:
			Serial.println("OTA: Receive Failed");
			break;
			case OTA_END_ERROR:
			Serial.println("OTA: End Failed");
			break;
		}
	});
}

void startOTA() {
	ArduinoOTA.begin();
	OTARunning = true;
}

String throttleOneIndex = "1";
String throttleOneCab = "0";
String throttleOneDirection = "1";
String throttleOneSpeed = "0";
String throttleTwoIndex = "2";
String throttleTwoCab = "0";
String throttleTwoDirection = "1";
String throttleTwoSpeed = "0";
void onWSEvent(AsyncWebSocket * server, AsyncWebSocketClient * client,
		AwsEventType type, void * arg, uint8_t *data, size_t len) {
	UNUSED(server);
	UNUSED(arg);
	if (type == WS_EVT_CONNECT) {
		client->printf("Welcome. Server Ready %u", client->id());
	} else if (type == WS_EVT_DATA) {
		String message = String((char *)data);
		message[len] = '\0';
		if (message == "ON") {
			DCCppPendingCommands.push("<1>");
			webSocket.textAll("Power ON.");
		} else if (message == "OFF") {
			DCCppPendingCommands.push("<0>");
			webSocket.textAll("Power OFF.");
		} else if (message == "S") {
			DCCppPendingCommands.push("<s>");
		} else if (message == "STOP1") {
			DCCppPendingCommands.push(String("<t") + throttleOneIndex + String(" ") + throttleOneCab + String(" -1 ") + throttleOneDirection + String(">"));
			webSocket.textAll("Emergency Stop T1.");
		} else if (message == "STOP2") {
			DCCppPendingCommands.push(String("<t") + throttleTwoIndex + String(" ") + throttleTwoCab + String(" -1 ") + throttleTwoDirection + String(">"));
			webSocket.textAll("Emergency Stop T2.");
		} else if (message.startsWith("CAB1")) {
			throttleOneCab = message.substring(5);
			webSocket.textAll("New Cab# T1: " + throttleOneCab);
		} else if (message.startsWith("CAB2")) {
			throttleTwoCab = message.substring(5);
			webSocket.textAll("New Cab# T2: " + throttleTwoCab);
		} else if (message == "FOR") {
			throttleOneDirection = "1";
			webSocket.textAll("Forward T1.");
			if (throttleOneSpeed != "0") {
				DCCppPendingCommands.push(String("<t") + throttleOneIndex + String(" ") + throttleOneCab + String(" ") + throttleOneSpeed + String(" ") + throttleOneDirection + String(">"));
			}
		} else if (message == "FOR2") {
			throttleTwoDirection = "1";
			webSocket.textAll("Forward T2.");
			if (throttleTwoSpeed != "0") {
				DCCppPendingCommands.push(String("<t") + throttleTwoIndex + String(" ") + throttleTwoCab + String(" ") + throttleTwoSpeed + String(" ") + throttleTwoDirection + String(">"));
			}
		} else if (message == "REV") {
			throttleOneDirection = "0";
			webSocket.textAll("Reverse T1.");
			if (throttleOneSpeed != "0") {
				DCCppPendingCommands.push(String("<t") + throttleOneIndex + String(" ") + throttleOneCab + String(" ") + throttleOneSpeed + String(" ") + throttleOneDirection + String(">"));
			}
		} else if (message == "REV2") {
			throttleTwoDirection = "0";
			webSocket.textAll("Reverse T2.");
			if (throttleTwoSpeed != "0") {
				DCCppPendingCommands.push(String("<t") + throttleTwoIndex + String(" ") + throttleTwoCab + String(" ") + throttleTwoSpeed + String(" ") + throttleTwoDirection + String(">"));
			}
		} else if (message.startsWith("T1")) {
			throttleOneSpeed = message.substring(3);
			DCCppPendingCommands.push(String("<t") + throttleOneIndex + String(" ") + throttleOneCab + String(" ") + throttleOneSpeed + String(" ") + throttleOneDirection + String(">"));
		} else if (message.startsWith("T2")) {
			throttleTwoSpeed = message.substring(3);
			DCCppPendingCommands.push(String("<t") + throttleTwoIndex + String(" ") + throttleTwoCab + String(" ") + throttleTwoSpeed + String(" ") + throttleTwoDirection + String(">"));
		} else if (message.startsWith("FnT1")) {
			String function = message.substring(4);
			DCCppPendingCommands.push(String("<f ") + throttleOneCab + String(" ") + function + String(">"));
			webSocket.textAll("Fn T1: " + function);
		} else if (message.startsWith("FnT2")) {
			String function = message.substring(4);
			DCCppPendingCommands.push(String("<f ") + throttleTwoCab + String(" ") + function + String(">"));
			webSocket.textAll("Fn T2: " + function);
		}
	}
}

void handleNotFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "FileNotFound");
}

const String espInfoTemplate = String("<html><head><title>ESP-DCC++ Information</title>"
		"<style>table, th, td { border: 1px solid black; border-collapse: collapse; } th, td { align: left }</style>"
		"</head>"
		"<body>"
		"<a href=\"/\">Home</a> "
		"<a href=\"/throttle\">DCC++ Web Throttle</a> "
		"<a href=\"/status\">DCC++ Status</a> "
		"<h1>Version Information</h1>"
		"<table>"
		"<tr><th>Component</th><th>Version</th></tr>"
		"<tr><td>ESP Core</td><td>@@CORE_VERSION@@</td></tr>"
		"<tr><td>ESP Boot Loader</td><td>@@BOOT_VERSION@@</td></tr>"
		"<tr><td>ESP SDK</td><td>@@SDK_VERSION@@</td></tr>"
		"<tr><td>ESP-DCC++ Build</td><td>" __DATE__ " " __TIME__ "</td></tr>"
		"</table>"
		"<h1>System Information</h1>"
		"<table>"
		"<tr><th>Detail</th><th>Value</th></tr>"
		"<tr><td>Available Heap Space</td><td>@@HEAP_FREE@@ bytes</td></tr>"
		"<tr><td>Available Sketch Space</td><td>@@SKETCH_FREE@@ bytes</td></tr>"
		"<tr><td>ESP Chip ID</td><td>@@CHIP_ID@@</td></tr>"
		"<tr><td>CPU Speed</td><td>@@CPU_SPEED@@Mhz</td></tr>"
		"<tr><td>Flash Size</td><td>@@FLASH_SIZE@@ bytes</td></tr>"
		"</table>"
		"</body></html>");

void handleESPInfo(AsyncWebServerRequest *request) {
	String response = espInfoTemplate;
	response.replace(String("@@CORE_VERSION@@"), String(ESP.getCoreVersion()));
	response.replace(String("@@BOOT_VERSION@@"), String(ESP.getBootVersion()));
	response.replace(String("@@SDK_VERSION@@"), String(ESP.getSdkVersion()));
	response.replace(String("@@HEAP_FREE@@"), String(ESP.getFreeHeap()));
	response.replace(String("@@SKETCH_FREE@@"), String(ESP.getFreeSketchSpace()));
	response.replace(String("@@CHIP_ID@@"), String(ESP.getChipId()));
	response.replace(String("@@CPU_SPEED@@"), String(ESP.getCpuFreqMHz()));
	response.replace(String("@@FLASH_SIZE@@"), String(ESP.getFlashChipSize()));

	request->send(200, "text/html", response);
}

const String indexTemplate = String("<html><head><title>ESP-DCC++</title>"
		"</head>"
		"<body><h1>ESP-DCC++ WiFi</h1>"
		"<a href=\"/espinfo\">ESP System Information</a> "
		"<a href=\"/throttle\">DCC++ Web Throttle</a> "
		"<a href=\"/status\">DCC++ Status</a> "
		"</body></html>");

void handleIndex(AsyncWebServerRequest *request) {
	String response = indexTemplate;
	request->send(200, "text/html", response);
}

const String dccPPStatusTemplate = String("<html><head><title>DCC++ Status</title>"
		"</head>"
		"<body><h1>DCC++ Status</h1>"
		"<a href=\"/\">Home</a>"
		"<a href=\"/espinfo\">ESP System Information</a> "
		"<a href=\"/throttle\">DCC++ Web Throttle</a> "
		"<h1>coming soon</h1></body></html>");

void handleDCCppStatus(AsyncWebServerRequest *request) {
	String response = dccPPStatusTemplate;
	request->send(200, "text/html", response);
}

void setup() {
	WiFi.setAutoConnect(false);

	if (OTA_ENABLED) {
		setupOTA();
	}
	SERIAL_LINK_DEV.begin(SERIAL_LINK_SPEED);
	SERIAL_LINK_DEV.flush();
	SERIAL_LINK_DEV.println("<iESP-DCC++ init>");

	currentDCCppCommand.reserve(128);

	webSocket.onEvent(onWSEvent);
	webServer.addHandler(&webSocket);
	webServer.addHandler(new SPIFFSEditor());

	webServer.on("/espinfo", HTTP_GET, &handleESPInfo);
	webServer.on("/status", HTTP_GET, &handleDCCppStatus);
	webServer.on("/", &handleIndex);
	webServer.serveStatic("/throttle", SPIFFS, "/").setDefaultFile("index.html");
	webServer.onNotFound(&handleNotFound);

	SPIFFS.begin();
}

void loop() {
	if (OTARunning) {
		ArduinoOTA.handle();
	}
	if (DCCppServer.status() == LISTEN) {
		if (DCCppServer.hasClient()) {
			for (int i = 0; i < MAX_DCCPP_CLIENTS; i++) {
				if (!DCCppClients[i] || !DCCppClients[i].connected()) {
					if (DCCppClients[i]) {
						DCCppClients[i].stop();
					}
					DCCppClients[i] = DCCppServer.available();
					continue;
				}
			}
			DCCppServer.available().stop();
		}
		//check clients for data
		for (int i = 0; i < MAX_DCCPP_CLIENTS; i++) {
			if (DCCppClients[i] && DCCppClients[i].connected()) {
				if (DCCppClients[i].available()) {
					size_t len = DCCppClients[i].available();
					char sbuf[len + 1];
					char cmdBuf[len + 1];
					DCCppClients[i].peekBytes(sbuf, len);
					sbuf[len] = '\0';
					size_t baseOffs = 0;
					for(size_t idx = 0; idx < len; idx++) {
						if(sbuf[baseOffs] == '<' && sbuf[idx] == '>') {
							// we have a complete command string, len is +1 to capture the >
							size_t bytesRead = DCCppClients[i].readBytes(cmdBuf, idx - baseOffs + 1);

							// store this so we can read off chunks later
							baseOffs = idx;

							// make sure we null terminate the string
							cmdBuf[bytesRead] = '\0';

							// submit command to DCC++ pending queue...
							DCCppPendingCommands.push(String(cmdBuf));
							//SERIAL_LINK_DEV.println("IN CMD:" + String(cmdBuf));
						} else if (sbuf[baseOffs] == '\r' || sbuf[baseOffs] == '\n') {
							// store this so we can read off chunks later
							baseOffs = idx;
							// discard the \r or \n
							DCCppClients[i].read();
						}
					}
				}
			}
		}
	}
	//check UART for data
	while(SERIAL_LINK_DEV.available()) {
		char ch = SERIAL_LINK_DEV.read();
		if(!inDCCPPCommand && ch == '<') {
			inDCCPPCommand = true;
		}
		if(inDCCPPCommand) {
			currentDCCppCommand += ch;
		}
		if(inDCCPPCommand && ch == '>') {
			inDCCPPCommand = false;
			if(currentDCCppCommand.startsWith("<iESP-")) {
				if(currentDCCppCommand.indexOf("connect") > 0) {
					size_t firstSpace = currentDCCppCommand.indexOf(' ');
					size_t secondSpace = currentDCCppCommand.indexOf(' ', firstSpace + 1);
					long ssidLength = currentDCCppCommand.substring(firstSpace + 1, secondSpace).toInt();
					firstSpace = secondSpace++;
					secondSpace += ssidLength;
					String ssid = currentDCCppCommand.substring(firstSpace + 1, secondSpace);
					String password = currentDCCppCommand.substring(secondSpace + 1, currentDCCppCommand.length() - 1);
					WiFi.setAutoReconnect(false);
					SERIAL_LINK_DEV.print("<iESP connecting to ");
					SERIAL_LINK_DEV.print(ssid);
					SERIAL_LINK_DEV.println(">");
					WiFi.begin(ssid.c_str(), password.c_str());
					bool connecting = true;
					// ~30sec timeout
					int connectTimeout = 120;
					while(connecting && connectTimeout) {
						connectTimeout--;
						delay(250);
						switch(WiFi.status()) {
							case WL_CONNECTED:
								SERIAL_LINK_DEV.print("<iESP connected ");
								SERIAL_LINK_DEV.print(WiFi.localIP());
								SERIAL_LINK_DEV.println(">");
								WiFi.setAutoReconnect(true);
								connecting = false;
								break;
							case WL_CONNECT_FAILED:
								SERIAL_LINK_DEV.println("<iESP connect failed>");
								connecting = false;
								break;
							case WL_DISCONNECTED:
								// ignoring this as we get this status until the it either succeeds or fails.
								break;
							case WL_NO_SSID_AVAIL:
								SERIAL_LINK_DEV.println("<iESP AP not found>");
								connecting = false;
								break;
							default:
								SERIAL_LINK_DEV.print("<iESP status ");
								SERIAL_LINK_DEV.print(WiFi.status());
								SERIAL_LINK_DEV.println(">");
								break;
						}
					}
					if(WiFi.status() != WL_CONNECTED && !connectTimeout) {
						SERIAL_LINK_DEV.println("<iESP connect timeout>");
					}
				} else if(currentDCCppCommand.indexOf("start") > 0) {
					DCCppServer.begin();
					DCCppServer.setNoDelay(true);
					webServer.begin();
					SERIAL_LINK_DEV.println("<iESP ready>");
				} else if(currentDCCppCommand.indexOf("stop") > 0) {
					DCCppServer.stop();
					webServer.end();
					SERIAL_LINK_DEV.println("<iESP shutdown>");
				} else if(currentDCCppCommand.indexOf("reset") > 0) {
					ESP.restart();
				} else if(currentDCCppCommand.indexOf("ip") > 0) {
					SERIAL_LINK_DEV.print("<iESP ip ");
					if(WiFi.status() == WL_CONNECTED) {
						SERIAL_LINK_DEV.print(WiFi.localIP());
					} else {
						SERIAL_LINK_DEV.print("0.0.0.0");
					}
					SERIAL_LINK_DEV.println(">");
				} else if(currentDCCppCommand.indexOf("scan") > 0) {
					int networkCount = WiFi.scanNetworks();
					for(int net = 0; net < networkCount; net++) {
						SERIAL_LINK_DEV.print("<iESP-network: ");
						SERIAL_LINK_DEV.print(WiFi.SSID(net));
						SERIAL_LINK_DEV.println(">");
					}
				} else if(currentDCCppCommand.indexOf("status") > 0) {
					SERIAL_LINK_DEV.print("<iESP-status ");
					SERIAL_LINK_DEV.print(WiFi.status());
					SERIAL_LINK_DEV.print(" ");
					if(WiFi.status() == WL_CONNECTED) {
						SERIAL_LINK_DEV.print(WiFi.localIP());
					} else {
						SERIAL_LINK_DEV.print("0.0.0.0");
					}
					SERIAL_LINK_DEV.print(" ");
					SERIAL_LINK_DEV.print(DCCppServer.status());
					SERIAL_LINK_DEV.println(">");
				}
			}
			if (DCCppServer.status() == LISTEN) {
				for (int i = 0; i < MAX_DCCPP_CLIENTS; i++) {
					if (DCCppClients[i] && DCCppClients[i].connected()) {
						DCCppClients[i].print(currentDCCppCommand);
						delay(1);
					}
				}
				webSocket.textAll(currentDCCppCommand);
			}
			currentDCCppCommand = "";
		}
	}
	// drain the queued up commands
	while(DCCppPendingCommands.count() > 0) {
		SERIAL_LINK_DEV.print(DCCppPendingCommands.pop());
	}
}


