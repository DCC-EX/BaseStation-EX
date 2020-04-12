/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCppEX.h"
#include "CurrentMonitor.h"
#include "CommInterface.h"

///////////////////////////////////////////////////////////////////////////////

#define  CURRENT_SAMPLE_SMOOTHING          0.01

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)   // Configuration for UNO
  #define  CURRENT_SAMPLE_TIME        10
#else                                                       // Configuration for MEGA
  #define  CURRENT_SAMPLE_TIME        1
#endif

MotorBoard::MotorBoard(int sensePin, int enablePin, MOTOR_BOARD_TYPE type, const char *name) : sensePin(sensePin), 
																							   enablePin(enablePin), 
																							   name(name), 
																							   current(0), 
																							   reading(0),
																							   triggerMilliamps(0),
																							   maxMilliAmps(0), 
																							   triggered(false), 
																							   lastCheckTime(0) {
	switch(type) {
		case ARDUINO_SHIELD:
			// Board outputs 1.65V / Amp, Current conversion factor: ((5/1024)/1.65))*1000 = 2.96
			// Arduino motor board: 300 sensor reading == 890mA (300 * 2.96)
			triggerMilliamps = 890;
			maxMilliAmps = 2000;
			break;
		case POLOLU:
			// Board outputs .525V / Amp, Current conversion factor: ((5/1024)/.525))*1000 = 9.30
			// Pololu motor board: 160 sensor reading == 1.493A (160 * 9.30)/1000
			triggerMilliamps = 1490;
			maxMilliAmps = 3000;
			break;
		case BTS7960B_5A:
			// Board outputs ..0105V / Amp, Current conversion factot: ((5/1024) /.0105)*1000 = 465
			// BTS7960B motor board: 11 sensor reading == 5.133A (11 * 465)/1000
			triggerMilliamps = 5133;
			maxMilliAmps = 43000;
			break;
		case BTS7960B_10A:
			// Board outputs ..0105V / Amp, Current conversion factot: ((5/1024) /.0105)*1000 = 465
			// BTS7960B motor board: 22 sensor reading == 10.266A (22 * 465)/1000
			triggerMilliamps = 10266;
			maxMilliAmps = 43000;
			break;
		case LMD18200:
			// *** requires 2.2k resistor ***
			// resistor is calculated for 6A because that is the max the board reports. We don't want to 
			// send more than 5V to the Arduino GPIO
			// Board with resistor outputs .83V/A, Current conversion factor: ((5/1024)/.83)*1000 = 5.88
			// LM18200 motor board: 510 sensor reading == 3A (510 * 5.88)/1000
			triggerMilliamps = 3000;
			maxMilliAmps = 3000;
		case LMD18200_MAX471:
			// MAX471 outputs 1V/A, Current conversion factor is ((5/1024)/1)*1000 = 4.88
			// LMD18200 & MAX471 for curent sense: 615 sensor reading == 3A (615 * 4.88)/1000
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
	// return the raw Arduino pin reading
	return reading;
}

int MotorBoard::getLastCurrent() {
	// return true current in MilliAmps
	return current;
}

int MotorBoard::getTriggerMilliAmps() {
	// return the value that will trigger track shutoff for overcurrent
	return triggerMilliamps;
}

int MotorBoard::getMaxMilliAmps() {
	// returnt the maximum current handling capability of the motor board
	return maxMilliAmps;
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
				CommManager::printf("<a %d %d %d %d>", boards[0]->getLastRead(), boards[0]->getLastCurrent() , boards[0]->getTriggerMilliAmps(), boards[0]->getMaxMilliAmps());
			} else {
				for(int i = 0; i < MAX_MOTOR_BOARDS; i++) {
					if(boards[i] != NULL && strcasecmp(boards[i]->getName(), com+2) == 0) {
						CommManager::printf("<a %s %d %d %d>", boards[i]->getName(), boards[i]->getLastRead(), boards[0]->getLastCurrent() , boards[0]->getTriggerMilliAmps(), boards[0]->getMaxMilliAmps());
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
