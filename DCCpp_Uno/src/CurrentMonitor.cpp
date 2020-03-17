/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp.h"
#include "CurrentMonitor.h"
#include "CommInterface.h"

///////////////////////////////////////////////////////////////////////////////

#define  CURRENT_SAMPLE_SMOOTHING          0.01

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)   // Configuration for UNO
  #define  CURRENT_SAMPLE_TIME        10
#else                                                       // Configuration for MEGA
  #define  CURRENT_SAMPLE_TIME        1
#endif

MotorBoard::MotorBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, const char *name) : sensePin(sensePin), enablePin(enablePin), name(name), current(0), reading(0), triggered(false), lastCheckTime(0) {
	switch(type) {
		case ARDUINO_SHIELD:
			// Arduino motor board: 890mA == 300 sensor reading
			triggerMilliamps = 890;
			maxMilliAmps = 2000;
			break;
		case POLOLU:
			// Pololu motor board: 1.493A == 160 sensor reading
			triggerMilliamps = 1490;
			maxMilliAmps = 3000;
			break;
		case BTS7960B_5A:
			// BTS7960B motor board: 5.133A == 11 sensor reading
			triggerMilliamps = 5133;
			maxMilliAmps = 5000;
			break;
		case BTS7960B_10A:
			// BTS7960B motor board: 10.266A == 22 sensor reading
			triggerMilliamps = 10000;
			maxMilliAmps = 10000;
			break;
		case LMD18200_MAX471:
			// LMD18200 motor board: 3.0A == 22*0.0049/
			triggerMilliamps = 3000;
			maxMilliAmps = 3000;
			break;
	}
}

void MotorBoard::check() {
	// if we have exceeded the CURRENT_SAMPLE_TIME we need to check if we are over/under current.
	if(millis() - lastCheckTime > CURRENT_SAMPLE_TIME) {
		lastCheckTime = millis();
		reading = analogRead(sensePin) * CURRENT_SAMPLE_SMOOTHING + current * (1.0 - CURRENT_SAMPLE_SMOOTHING);
		current = reading * CURRENT_CONVERSION_FACTOR; // get current in milliamps
		if(current > triggerMilliamps && digitalRead(enablePin)) {
			powerOff(false, true);
			triggered=true;
		} else if(current < triggerMilliamps && triggered) {
			powerOn();
			triggered=false;
		}
	}
}

void MotorBoard::powerOn(bool announce) {
	digitalWrite(enablePin, HIGH);
	if(announce) {
		CommManager::printf("<p1 %s>", name);
	}
}

void MotorBoard::powerOff(bool announce, bool overCurrent) {
	digitalWrite(enablePin, LOW);
	if(announce) {
		if(overCurrent) {
			CommManager::printf("<p2 %s>", name);
		} else {
			CommManager::printf("<p0 %s>", name);
		}
	}
}

int MotorBoard::getLastRead() {
	return current;  //fnd returns milliamps with var "current" with my changes. may need it to return "reading" for JMRI
}

void MotorBoard::showStatus() {
	if(digitalRead(enablePin) == LOW) {
		CommManager::printf("<p0 %s>", name);
	} else {
		CommManager::printf("<p1 %s>", name);
	}
}

#if MAX_MOTOR_BOARDS > 6
MotorBoard *MotorBoardManager::boards[MAX_MOTOR_BOARDS] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#else
MotorBoard *MotorBoardManager::boards[MAX_MOTOR_BOARDS] = { NULL, NULL, NULL, NULL, NULL, NULL };
#endif

void MotorBoardManager::registerBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, const char *name) {
	for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] == NULL) {
			boards[i] = new MotorBoard(sensePin, enablePin, type, name);
			return;
		}
	}
}

void MotorBoardManager::check() {
	for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] != NULL) {
			boards[i]->check();
		}
	}
}

void MotorBoardManager::powerOnAll() {
	for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] != NULL) {
			boards[i]->powerOn(false);
		}
	}
	CommManager::printf("<p1>");
#if defined(ENABLE_LCD) && LCD_LINES > 2
	if(lcdEnabled) {
		lcdDisplay.setCursor(13, 3);
		lcdDisplay.print("ON ");
	}
#endif
}

void MotorBoardManager::powerOffAll() {
	for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] != NULL) {
			boards[i]->powerOff(false);
		}
	}
	CommManager::printf("<p0>");
#if defined(ENABLE_LCD) && LCD_LINES > 2
	if(lcdEnabled) {
		lcdDisplay.setCursor(13, 3);
		lcdDisplay.print("OFF");
	}
#endif
}

void MotorBoardManager::parse(const char *com) {
	switch(com[0]) {
		case '0':
			if(strlen(com) == 1) {
				powerOffAll();
			} else {
				for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
					if(boards[i] != NULL && strcasecmp(boards[i]->getName(), com+2) == 0) {
						boards[i]->powerOff();
						return;
					}
				}
				CommManager::printf("<X>");
			}
			break;
		case '1':
			if(strlen(com) == 1) {
				powerOnAll();
			} else {
				for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
					if(boards[i] != NULL && strcasecmp(boards[i]->getName(), com+2) == 0) {
						boards[i]->powerOn();
						return;
					}
				}
				CommManager::printf("<X>");
			}
			break;
		case 'c':
			if(strlen(com) == 1) {
				CommManager::printf("<a %d>", boards[0]->getLastRead());
			} else {
				for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
					if(boards[i] != NULL && strcasecmp(boards[i]->getName(), com+2) == 0) {
						CommManager::printf("<a %s %d>", boards[i]->getName(), boards[i]->getLastRead());
						return;
					}
				}
				CommManager::printf("<X>");
			}
			break;
	}
}

void MotorBoardManager::showStatus() {
	for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] != NULL) {
			boards[i]->showStatus();
		}
	}
}
