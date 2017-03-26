/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

enum MOTOR_BOARD_TYPE { ARDUINO_SHIELD, POLOLU, BTS7960B_5A, BTS7960B_10A };

// cap the number of motor boards at the maximum number of analog inputs
#define MAX_MOTOR_BOARDS NUM_ANALOG_INPUTS

class MotorBoard {
public:
	MotorBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, const char *name);
	void check();
	void powerOn(bool announce=true);
	void powerOff(bool announce=true, bool overCurrent=false);
	int getLastRead();
	void showStatus();
	const char *getName() {
		return name;
	}
private:
	int sensePin;
	int enablePin;
	const char *name;
	float current;
	bool triggered;
	long int lastCheckTime;
	int triggerValue;
};

class MotorBoardManager {
public:
	static void registerBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, const char *name);
	static void check();
	static void powerOnAll();
	static void powerOffAll();
	static void parse(const char *command);
	static void showStatus();
private:
	static MotorBoard *boards[MAX_MOTOR_BOARDS];
};

#endif

