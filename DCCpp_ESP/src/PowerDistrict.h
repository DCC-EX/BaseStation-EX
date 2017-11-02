/*
 * PowerDistrictStatus.h
 *
 *  Created on: Oct 24, 2017
 *      Author: mdunston
 */

#ifndef POWERDISTRICT_H_
#define POWERDISTRICT_H_

#include <WString.h>
#include <ArduinoJson.h>

class PowerDistrict {
	String _districtName;
	bool _currentState;
	int _powerUsage;
	bool _overCurrent;
	public:
		PowerDistrict(String name, bool state=false, int usage=0, bool fault=false);
		void toJSON(JsonObject &node);
		bool isCurrentState() const;
		const String& getDistrictName() const;
		bool isOverCurrent() const;
		void setOverCurrent(bool overCurrent);
		int getPowerUsage() const;
		void setPowerUsage(int powerUsage);
		void setCurrentState(bool currentState);

};

#endif /* POWERDISTRICT_H_ */
