/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ EX BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

enum MOTOR_BOARD_TYPE { ARDUINO_SHIELD, POLOLU, BTS7960B_5A, BTS7960B_10A, LMD18200, LMD18200_MAX471 };

// cap the number of motor boards to 2 for 1 MAIN and 1 PROG track.
// we have to be carfule on the Uno, but on a Mega, we could set this equal NUM_ANALOG_INPUTS
#define MAX_MOTOR_BOARDS 2

class MotorBoard {
public:
	MotorBoard(uint8_t sensePin, uint8_t enablePin, MOTOR_BOARD_TYPE type, int currentConvFactor, bool isProgTrack, const char *name);
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
	uint8_t sensePin;
	uint8_t enablePin;
	const char *name;
	float current;              // converted current in milliAmps
	float reading;              // raw current sense pin reading
	float maxCurrent;           // max current the board can handle in milliAmps
	float tripCurrent;          // computed trip current in milliAmps
	int tripCurrentReading;     // the pin reading current that will trip the overcurrent condition
	int tripCurrentMilliAmps;   // overcurrent user setting in milliAmps
	int maxCurrentReading;      // the pin reading for maximum current supported by the motor board
	int maxCurrentMilliamps;    // the user entered max current in milliamps
	int currentConvFactor;      // a multiplier constant to get current from pin reading
	bool tripped;
	unsigned long int lastCheckTime;
	unsigned long int lastTripTime;
};

class MotorBoardManager {
public:
	static void registerBoard(uint8_t sensePin, uint8_t enablePin, MOTOR_BOARD_TYPE type, int currentConvFactor, bool isProgTrack, const char *name);
	static void check();
	static void powerOnAll();
	static void powerOffAll();
	static void parse(const char *command);
	static void showStatus();
private:
	static MotorBoard *boards[MAX_MOTOR_BOARDS];
};

#endif