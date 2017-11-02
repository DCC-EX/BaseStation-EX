/*
 * ProgramRequest.h
 *
 *  Created on: Oct 26, 2017
 *      Author: mdunston
 */

#ifndef PROGRAMREQUEST_H_
#define PROGRAMREQUEST_H_

#include <ArduinoJson.h>

class ProgramRequest {
	int _callbackNumber;
	int _callbackSubNumber;
	int _value;
	int _cv;
	long _age;
public:
	ProgramRequest(int cv, int callbackNumber, int callbackSubNumber);
	void toJSON(JsonObject &node);
	int getCallbackNumber() const;
	int getCallbackSubNumber() const;
	int getValue() const;
	void setValue(int value);
	int getCv() const;
	void setCv(int cv);
	long getAge() const;
};

#endif /* PROGRAMREQUEST_H_ */
