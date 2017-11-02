/*
 * Output.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#include "Output.h"
#include <ArduinoJson.h>

Output::Output(int id, int pin, bool inverted, bool active) : _id(id), _pin(pin), _inverted(inverted), _active(active) {

}

void Output::toJSON(JsonObject &node) {
	node["id"] = _id;
	if(_active) {
		node["state"] = F("On");
	} else {
		node["state"] = F("Off");
	}
	node["pin"] = _pin;
	node["inverted"] =  _inverted;
}

bool Output::isActive() const {
	return _active;
}

void Output::setActive(bool active) {
	_active = active;
}

int Output::getId() const {
	return _id;
}

bool Output::isInverted() const {
	return _inverted;
}

int Output::getPin() const {
	return _pin;
}
