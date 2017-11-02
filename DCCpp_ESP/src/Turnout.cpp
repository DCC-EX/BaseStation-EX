/*
 * Turnout.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#include "Turnout.h"
#include <ArduinoJson.h>

Turnout::Turnout(int id, int address, int subAddress, bool thrown) : _id(id), _address(address), _subAddress(subAddress), _thrown(thrown) {

}

void Turnout::toJSON(JsonObject &node) {
	node["id"] = _id;
	node["address"] = _address;
	node["subAddress"] = _subAddress;
	if(_thrown) {
		node["state"] = F("Thrown");
	} else {
		node["state"] = F("Normal");
	}
}

int Turnout::getAddress() const {
	return _address;
}

int Turnout::getId() const {
	return _id;
}

int Turnout::getSubAddress() const {
	return _subAddress;
}

bool Turnout::isThrown() const {
	return _thrown;
}

void Turnout::setThrown(bool thrown) {
	_thrown = thrown;
}
