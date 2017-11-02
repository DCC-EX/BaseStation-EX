/*
 * PowerDistrict.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#include "PowerDistrict.h"
#include <ArduinoJson.h>

PowerDistrict::PowerDistrict(String name, bool state, int usage, bool fault) :
		_districtName(name), _currentState(state), _powerUsage(usage), _overCurrent(
				fault) {

}

void PowerDistrict::toJSON(JsonObject &node) {
	node[F("name")] = _districtName;
	if (!_currentState && _overCurrent) {
		node[F("state")] = F("Fault");
	} else if (!_currentState) {
		node[F("state")] = F("Off");
	} else if (_currentState) {
		node[F("state")] = F("Normal");
	}
	node[F("usage")] = _powerUsage;
}

bool PowerDistrict::isCurrentState() const {
	return _currentState;
}

const String& PowerDistrict::getDistrictName() const {
	return _districtName;
}

bool PowerDistrict::isOverCurrent() const {
	return _overCurrent;
}

void PowerDistrict::setOverCurrent(bool overCurrent) {
	_overCurrent = overCurrent;
}

int PowerDistrict::getPowerUsage() const {
	return _powerUsage;
}

void PowerDistrict::setPowerUsage(int powerUsage) {
	_powerUsage = powerUsage;
}

void PowerDistrict::setCurrentState(bool currentState) {
	_currentState = currentState;
}
