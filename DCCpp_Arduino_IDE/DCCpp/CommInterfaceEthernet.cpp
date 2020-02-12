/*
 * CommInterfaceEthernet.cpp
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#include <Arduino.h>
#include "DCCpp.h"
#include "CommInterface.h"
#include "CommInterfaceEthernet.h"
#include "SerialCommand.h"

#if COMM_INTERFACE >= 1 && COMM_INTERFACE <= 3
EthernetInterface::EthernetInterface() : server(ETHERNET_PORT), buffer(""), inCommandPayload(false) {
	pinMode(SDCARD_CS, OUTPUT);
	digitalWrite(SDCARD_CS, HIGH);     // Deselect the SD card
	byte localMac[] = MAC_ADDRESS;
	for(int i = 0; i <= 5; i++) {
		mac[i] = localMac[i];
	}
#ifdef IP_ADDRESS
	Ethernet.begin(mac, IP_ADDRESS);   // Start networking using STATIC IP Address
#else
	Ethernet.begin(mac);               // Start networking using DHCP to get an IP Address
#endif
	server.begin();
#ifdef ENABLE_LCD
	if(lcdEnabled) {
		lcdDisplay.setCursor(0, 1);
		lcdDisplay.print(Ethernet.localIP());
	}
#endif
}

void EthernetInterface::process() {
	EthernetClient client = server.available();
	if(client && client.connected()) {
		char ch = client.read();
		if (ch == '<') {
			inCommandPayload = true;
			buffer = "";
		} else if (ch == '>') {
			SerialCommand::parse(buffer.c_str());
			buffer = "";
			inCommandPayload = false;
		} else if(inCommandPayload) {
			buffer += ch;
		}
	}
}

void EthernetInterface::showConfiguration() {
	Serial.print("Ethernet Shield: ");
	Serial.print("MAC: ");
	for(int i = 0; i < 5; i++){
		Serial.print(mac[i], HEX);
		Serial.print(":");
	}
	Serial.print(mac[5], HEX);
	Serial.print(" - IP: ");
	Serial.print(Ethernet.localIP());
	Serial.print(" - PORT: ");
	Serial.println(ETHERNET_PORT);
}

void EthernetInterface::showInitInfo() {
	IPAddress localAddress = Ethernet.localIP();
	CommManager::printf("<N1: %d.%d.%d.%d>", localAddress[0], localAddress[1], localAddress[2], localAddress[3]);
}

void EthernetInterface::send(const char *buf) {
	server.print(buf);
}

#endif
