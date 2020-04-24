/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

enum MOTOR_BOARD_TYPE { ARDUINO_SHIELD, POLOLU, BTS7960B_5A, BTS7960B_10A, LMD18200, LMD18200_MAX471 };

// cap the number of motor boards at the maximum number of analog inputs
#define MAX_MOTOR_BOARDS NUM_ANALOG_INPUTS

class MotorBoard {
public:
	MotorBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, int currentConvFactor, bool isProgTrack, const char *name);
	void check();
	void powerOn(bool announce=true);
	void powerOff(bool announce=true, bool overCurrent=false);
	int getLastRead();
	int getLastCurrent();
	int getTripMilliAmps();
	int getMaxMilliAmps();
	void showStatus();
	const char *getName() {
		return name;
	}
private:
	int sensePin;
	int enablePin;
	const char *name;
	float current;             // converted current in milliAmps
	float reading;             // raw current pin reading
	int tripMilliamps;         // the current that will trip the overcurrent condition
	int maxMilliAmps;          // the maximum current supported by the motor board
	int currentConvFactor;     // a multiplier constant to get current from pin reading
	bool tripped;
	long int lastCheckTime;
};

class MotorBoardManager {
public:
	static void registerBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, int currentConvFactor, bool isProgTrack, const char *name);
	static void check();
	static void powerOnAll();
	static void powerOffAll();
	static void parse(const char *command);
	static void showStatus();
private:
	static MotorBoard *boards[MAX_MOTOR_BOARDS];
};

#endif