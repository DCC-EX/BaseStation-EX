/*
 * Turnout.h
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#ifndef TURNOUT_H_
#define TURNOUT_H_

#include <ArduinoJson.h>

class Turnout {
	int _id;
	int _address;
	int _subAddress;
	bool _thrown;
public:
	Turnout(int id, int address, int subAddress, bool thrown=false);
	void toJSON(JsonObject &node);
	int getAddress() const;
	int getId() const;
	int getSubAddress() const;
	bool isThrown() const;
	void setThrown(bool thrown);
};

#endif /* TURNOUT_H_ */
