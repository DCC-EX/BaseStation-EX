/*
 * Sensor.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#include "Sensor.h"
#include <ArduinoJson.h>

Sensor::Sensor(int id, int pin, bool active, bool pullUp) :
		_id(id), _pin(pin), _active(active), _pullUp(pullUp) {
}

void Sensor::toJSON(JsonObject &node) {
	node["id"] = _id;
	node["state"] = _active;
	node["pin"] = _pin;
	node["pullUp"] = _pullUp;
}

bool Sensor::isActive() const {
	return _active;
}

void Sensor::setActive(bool active) {
	_active = active;
}

int Sensor::getId() const {
	return _id;
}

int Sensor::getPin() const {
	return _pin;
}

bool Sensor::isPullUp() const {
	return _pullUp;
}
