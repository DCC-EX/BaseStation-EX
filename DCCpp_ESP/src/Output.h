/*
 * Output.h
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <ArduinoJson.h>

class Output {
	int _id;
	int _pin;
	bool _inverted;
	bool _active;
public:
	Output(int id, int pin, bool inverted=false, bool active=false);
	void toJSON(JsonObject &node);
	bool isActive() const;
	void setActive(bool active);
	int getId() const;
	bool isInverted() const;
	int getPin() const;
};

#endif /* OUTPUT_H_ */
