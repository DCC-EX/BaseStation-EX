/*
 * CommInterfaceSerial.cpp
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#include <Arduino.h>
#include "CommInterfaceSerial.h"
#include "SerialCommand.h"

HardwareSerialInterface::HardwareSerialInterface(HardwareSerial &serial, long baud) : serialStream(serial), baud(baud), buffer(""), inCommandPayload(false) {
	serial.begin(baud);
	serial.flush();
}

void HardwareSerialInterface::process() {
	while(Serial.available()) {
		char ch = Serial.read();
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

void HardwareSerialInterface::showConfiguration() {
	Serial.print(F("Hardware Serial - Speed:"));
	Serial.println(baud);
}

void HardwareSerialInterface::showInitInfo() {
	CommManager::printf("<N0:SERIAL>");
}

void HardwareSerialInterface::send(const char *buf) {
	Serial.print(buf);
}
