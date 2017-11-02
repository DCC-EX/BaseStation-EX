/*
 * CommInterfaceESP.cpp
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#include <Arduino.h>
#include "CommInterfaceESP.h"
#include "SerialCommand.h"
#include "DCCpp.h"

#if COMM_INTERFACE == 4

ESPInterface::ESPInterface(Stream &serialStream, IPAddress *ip) : _serialStream(serialStream), _buffer(""), _inCommandPayload(false) {
	init(ip);
}

void ESPInterface::init(IPAddress *ip) {
	int attempts = 10;
	bool responseFound = false;
	// set timeout to 500ms for reads
	_serialStream.setTimeout(500L);
	while(attempts-- > 0 && !responseFound) {
		_serialStream.println("<iESP-reset>");
		// give ESP some time to restart and stabilize
		delay(250);
		String resetStatus = _serialStream.readStringUntil('>');
		if(resetStatus.indexOf("iESP-DCC++ init") >= 0) {
			responseFound = true;
		}
	}
	_serialStream.println("<iESP-status>");
	String espStatus = _serialStream.readStringUntil('>');
	if(ip != NULL) {
		// TODO: Start networking using STATIC IP Address
	}
	_serialStream.print("<iESP-connect ");
	_serialStream.print(strlen(WIFI_SSID));
	_serialStream.print(" ");
	_serialStream.print(WIFI_SSID);
	_serialStream.print(" ");
	_serialStream.print(WIFI_PASSWORD);
	_serialStream.println(">");
	// discard the connecting response
	espStatus = _serialStream.readStringUntil('>');
	bool connecting = true;
	while (connecting) {
		// wait for the connected/failed message
		espStatus = _serialStream.readStringUntil('>');
		if (espStatus.indexOf("connected") > 0) {
			connecting = false;
			// connected to AP, parse the IP address out
			String ipString = espStatus.substring(
					espStatus.lastIndexOf(' ') + 1, espStatus.length());
			//Serial.println("IP: " + ipString);
			_localAddress.fromString(ipString);
		} else if (espStatus.indexOf("connect failed") > 0
				|| espStatus.indexOf("not found") > 0
				|| espStatus.indexOf("timeout") > 0) {
			// connection failed.  Abort.
			Serial.println("<iDCC++ Unable to connect to WiFi, Halting>");
			Serial.println(espStatus);
			// don't continue
			while (true)
				;
		}
	}
	_serialStream.println("<iESP-status>");
	_serialStream.readStringUntil('>');
	_serialStream.println("<iESP-start>");
	_serialStream.readStringUntil('>');
#ifdef ENABLE_LCD
	if(lcdEnabled) {
		lcdDisplay.setCursor(0, 1);
		lcdDisplay.print(_localAddress);
	}
#endif

}

void ESPInterface::process() {
	while(_serialStream.available()) {
		char ch = _serialStream.read();
		if (ch == '<') {
			_inCommandPayload = true;
			_buffer = "";
		} else if (ch == '>') {
			SerialCommand::parse(_buffer.c_str());
			_buffer = "";
			_inCommandPayload = false;
		} else if(_inCommandPayload) {
			_buffer += ch;
		}
	}
}

void ESPInterface::showConfiguration() {
	Serial.print(" - IP: ");
	Serial.print(_localAddress);
	Serial.println(" (DHCP)");
}

void ESPInterface::showInitInfo() {
	CommManager::printf("<N1: %d.%d.%d.%d>", _localAddress[0], _localAddress[1], _localAddress[2], _localAddress[3]);
}

void ESPInterface::send(const char *buf) {
	_serialStream.print(buf);
}

ESPHardwareSerialInterface::ESPHardwareSerialInterface(HardwareSerial &serial, long baud, IPAddress *ip) : ESPInterface(initSerialStream(serial, baud), ip), _baud(baud), _serial(serial) {
}

void ESPHardwareSerialInterface::showConfiguration() {
	Serial.print("ESP - HardwareSerial - Speed:");
	Serial.print(_baud);
	ESPInterface::showConfiguration();
}

Stream &ESPHardwareSerialInterface::initSerialStream(HardwareSerial &serial, long baud) {
	serial.begin(baud);
	serial.flush();
	return serial;
}

ESPSoftwareSerialInterface::ESPSoftwareSerialInterface(int rxPin, int txPin, long baud, IPAddress *ip) : ESPInterface(initSerialStream(rxPin, txPin, baud), ip), _baud(baud), _rxPin(rxPin), _txPin(txPin) {
}

void ESPSoftwareSerialInterface::showConfiguration() {
	Serial.print("ESP - SoftwareSerial - Speed:");
	Serial.print(_baud);
	Serial.print(" - RX: ");
	Serial.print(_rxPin);
	Serial.print(" - TX: ");
	Serial.print(_txPin);
	ESPInterface::showConfiguration();
}

Stream &ESPSoftwareSerialInterface::initSerialStream(int rxPin, int txPin, long baud) {
	_serial = new SoftwareSerial(rxPin, txPin);
	_serial->begin(baud);
	_serial->flush();
	return *_serial;
}

#endif
