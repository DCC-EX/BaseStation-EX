/*
 * ProgramRequest.cpp
 *
 *  Created on: Oct 26, 2017
 *      Author: mdunston
 */

#include "Esp.h"
#include "ProgramRequest.h"
#include <ArduinoJson.h>

ProgramRequest::ProgramRequest(int cv, int callbackNumber,
		int callbackSubNumber) :
		_callbackNumber(callbackNumber), _callbackSubNumber(callbackSubNumber), _value(
				-1), _cv(cv), _age(millis()) {
}

void ProgramRequest::toJSON(JsonObject &node) {
	node.set("callback", _callbackNumber);
	node.set("subNumber", _callbackSubNumber);
	node.set("value", _value);
	node.set("cv", _cv);
}

int ProgramRequest::getCallbackNumber() const {
	return _callbackNumber;
}

int ProgramRequest::getCallbackSubNumber() const {
	return _callbackSubNumber;
}

int ProgramRequest::getValue() const {
	return _value;
}

void ProgramRequest::setValue(int value) {
	_value = value;
}

int ProgramRequest::getCv() const {
	return _cv;
}

void ProgramRequest::setCv(int cv) {
	_cv = cv;
}

long ProgramRequest::getAge() const {
	return _age;
}
