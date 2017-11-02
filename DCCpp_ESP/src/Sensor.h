/*
 * Sensor.h
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <ArduinoJson.h>

class Sensor {
	int _id;
	int _pin;
	bool _active;
	bool _pullUp;

public:
	Sensor(int id, int pin, bool active=false, bool pullUp=false);
	void toJSON(JsonObject &node);
	bool isActive() const;
	void setActive(bool active);
	int getId() const;
	int getPin() const;
	bool isPullUp() const;
};

#endif /* SENSOR_H_ */
