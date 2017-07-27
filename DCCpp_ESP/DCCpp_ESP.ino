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
#include <ESP8266mDNS.h>
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

bool inDCCPPCommand = false;

struct WebSocketClient {
	bool used;
	uint32_t id;
	String partialCommand;
};
WebSocketClient webSocketClients[MAX_WEBSOCKET_CLIENTS];

void onWSEvent(AsyncWebSocket * server, AsyncWebSocketClient * client,
		AwsEventType type, void * arg, uint8_t *data, size_t len) {
	UNUSED(server);
	UNUSED(arg);
	if (type == WS_EVT_CONNECT) {
		int clientIndex = -1;
		for(int index = 0; index < MAX_WEBSOCKET_CLIENTS; index++) {
			if(!webSocketClients[index].used && clientIndex == -1) {
				webSocketClients[index].used = true;
				webSocketClients[index].id = client->id();
				clientIndex = index;
			}
		}
		client->printf("Welcome. Server Ready %u:%u", client->id(), clientIndex);
	} else if (type == WS_EVT_DISCONNECT) {
		for(int index = 0; index < MAX_WEBSOCKET_CLIENTS; index++) {
			if(webSocketClients[index].id == client->id()) {
				webSocketClients[index].used = false;
				webSocketClients[index].partialCommand = "";
			}
		}
	} else if (type == WS_EVT_DATA) {
		int clientIndex = 0;
		for(int index = 0; index < MAX_WEBSOCKET_CLIENTS; index++) {
			if(webSocketClients[index].id == client->id()) {
				clientIndex = index;
			}
		}
		String message = String((char *)data);
		message[len] = '\0';
		webSocketClients[clientIndex].partialCommand += message;
		size_t baseOffs = 0;
		for(size_t index = 0; index <= webSocketClients[clientIndex].partialCommand.length(); index++) {
			if(webSocketClients[clientIndex].partialCommand[baseOffs] == '<' && webSocketClients[clientIndex].partialCommand[index] == '>') {
				DCCppPendingCommands.push(webSocketClients[clientIndex].partialCommand.substring(baseOffs, index + 1));
				baseOffs = index + 1;
			}
		}
		webSocketClients[clientIndex].partialCommand.remove(0, baseOffs);
	}
}

void handleNotFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "FileNotFound");
}

const String espInfoTemplate = String("["
		"{\"label\":\"ESP Core\",\"value\":\"@@CORE_VERSION@@\"},"
		"{\"label\":\"ESP Boot Loader\",\"value\":\"@@BOOT_VERSION@@\"},"
		"{\"label\":\"ESP SDK\",\"value\":\"@@SDK_VERSION@@\"},"
		"{\"label\":\"ESP-DCC++ Build\",\"value\":\"" __DATE__ " " __TIME__ "\"},"
		"{\"label\":\"Available Heap Space\",\"value\":\"@@HEAP_FREE@@\"},"
		"{\"label\":\"Available Sketch Space\",\"value\":\"@@SKETCH_FREE@@\"},"
		"{\"label\":\"ESP Chip ID\",\"value\":\"@@CHIP_ID@@\"},"
		"{\"label\":\"CPU Speed\",\"value\":\"@@CPU_SPEED@@Mhz\"},"
		"{\"label\":\"Flash Size\",\"value\":\"@@FLASH_SIZE@@ bytes\"}"
		"]");

void handleESPInfo(AsyncWebServerRequest *request) {
	String response = espInfoTemplate;
	response.replace(String("@@CORE_VERSION@@"), String(ESP.getCoreVersion()));
	response.replace(String("@@BOOT_VERSION@@"), String(ESP.getBootVersion()));
	response.replace(String("@@SDK_VERSION@@"), String(ESP.getSdkVersion()));
	response.replace(String("@@HEAP_FREE@@"), String(ESP.getFreeHeap()));
	response.replace(String("@@SKETCH_FREE@@"), String(ESP.getFreeSketchSpace()));
	response.replace(String("@@CHIP_ID@@"), String(ESP.getChipId(), HEX));
	response.replace(String("@@CPU_SPEED@@"), String(ESP.getCpuFreqMHz()));
	response.replace(String("@@FLASH_SIZE@@"), String(ESP.getFlashChipSize()));

	request->send(200, "application/json", response);
}

void setup() {
	WiFi.setAutoConnect(false);
	WiFi.hostname(HOSTNAME);

	currentDCCppCommand.reserve(128);

	webSocket.onEvent(onWSEvent);
	webServer.addHandler(&webSocket);
	webServer.addHandler(new SPIFFSEditor());

	webServer.on("/espinfo", HTTP_GET, &handleESPInfo);
	webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
	webServer.onNotFound(&handleNotFound);

	SPIFFS.begin();

	DCCppServer.setNoDelay(true);

	MDNS.addService("http", "tcp", 80);
	MDNS.addService("dccpp", "tcp", 2560);
	// TBD : WiThrottle support
	//MDNS.addService("_withrottle", "tcp", 81);
	//MDNS.addServiceTxt("_withrottle", "tcp", "jmri", "4.5.7");

	SERIAL_LINK_DEV.begin(SERIAL_LINK_SPEED);
	SERIAL_LINK_DEV.flush();
	SERIAL_LINK_DEV.println("<iESP-DCC++ init>");
}

void loop() {
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
					webServer.begin();
					MDNS.begin(HOSTNAME);
					SERIAL_LINK_DEV.println("<iESP ready>");
				} else if(currentDCCppCommand.indexOf("stop") > 0) {
					DCCppServer.stop();
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



