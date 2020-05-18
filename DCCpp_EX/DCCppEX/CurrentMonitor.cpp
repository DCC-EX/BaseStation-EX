/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ EX BASE STATION for the Arduino

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

MotorBoard::MotorBoard (uint8_t _sensePin, uint8_t _enablePin, MOTOR_BOARD_TYPE _type, int _currentConvFactor, bool _isProgTrack, const char *_name) {
	this->sensePin=_sensePin;
	this->enablePin=_enablePin;
	this->name=_name;
	this->current=0;
	this->reading=0;
	//this->triggerMilliamps=0; TODO clean this up
	//this->maxMilliAmps=0;
	this->currentConvFactor=_currentConvFactor;
	this->tripped=false;
	this->lastCheckTime=0; {

	switch(_type) {
		case ARDUINO_SHIELD:
			// Board outputs 1.65V / Amp, Current conversion factor: ((5/1024)/1.65))*1000 = 2.96
			// Arduino motor board: 300 sensor reading == 890mA (300 * 2.96)
			tripMilliamps = 1500;
			maxMilliAmps = 2000;
			break;
		case POLOLU:
			// Board outputs .525V / Amp, Current conversion factor: ((5/1024)/.525))*1000 = 9.30
			// Pololu motor board: 160 sensor reading == 1.493A (160 * 9.30)/1000
			tripMilliamps = 2500;
			maxMilliAmps = 3000;
			break;
		case BTS7960B_5A:
			// Board outputs ..0105V / Amp, Current conversion factot: ((5/1024) /.0105)*1000 = 465
			// BTS7960B motor board: 11 sensor reading == 5.133A (11 * 465)/1000
			tripMilliamps = 5133;
			maxMilliAmps = 43000;
			break;
		case BTS7960B_10A:
			// Board outputs ..0105V / Amp, Current conversion factot: ((5/1024) /.0105)*1000 = 465
			// BTS7960B motor board: 22 sensor reading == 10.266A (22 * 465)/1000
			tripMilliamps = 10266;
			maxMilliAmps = 43000;
			break;
		case LMD18200:
			// *** requires 2.2k resistor ***
			// resistor is calculated for 6A because that is the max the board reports. We don't want to 
			// send more than 5V to the Arduino GPIO
			// Board with resistor outputs .83V/A, Current conversion factor: ((5/1024)/.83)*1000 = 5.88
			// LM18200 motor board: 510 sensor reading == 3A (510 * 5.88)/1000
			tripMilliamps = 3000;
			maxMilliAmps = 3000;
		case LMD18200_MAX471:
			// MAX471 outputs 1V/A, Current conversion factor is ((5/1024)/1)*1000 = 4.88
			// LMD18200 & MAX471 for curent sense: 615 sensor reading == 3A (615 * 4.88)/1000
			tripMilliamps = 3000;
			maxMilliAmps = 3000;
			break;
		}
	}
	if(_isProgTrack) {
		tripMilliamps = 250;
	}
}

void MotorBoard::check() {
	// if we have exceeded the CURRENT_SAMPLE_TIME we need to check if we are over/under current.
	if(millis() - lastCheckTime > CURRENT_SAMPLE_TIME) { // TODO can we integrate this with the readBaseCurrent and ackDetect routines?
		lastCheckTime = millis();
		reading = analogRead(sensePin) * CURRENT_SAMPLE_SMOOTHING + reading * (1.0 - CURRENT_SAMPLE_SMOOTHING);
		current = (reading * currentConvFactor)/100; // get current in milliamps
		if(current > tripMilliamps && digitalRead(enablePin)) { // TODO convert this to integer match
			powerOff(false, true);
			tripped=true;
			lastTripTime=millis();
		} else if(current < tripMilliamps && tripped) { // TODO need to put a delay in here so it only tries after X seconds
			if (millis() - lastTripTime > 100000) {  // TODO make this a global constant
			  powerOn();
			  tripped=false;
			}
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

int MotorBoard::getTripMilliAmps() {
	// return the value that will trip track shutoff for overcurrent
	return tripMilliamps;
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

#if MAX_MOTOR_BOARDS > 2
MotorBoard *MotorBoardManager::boards[MAX_MOTOR_BOARDS] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#else
MotorBoard *MotorBoardManager::boards[MAX_MOTOR_BOARDS] = { NULL, NULL };
#endif

void MotorBoardManager::registerBoard(uint8_t sensePin, uint8_t enablePin, MOTOR_BOARD_TYPE type, int currentConvFactor, bool isProgTrack, const char *name) {
	for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] == NULL) {
			boards[i] = new MotorBoard(sensePin, enablePin, type, currentConvFactor, isProgTrack, name);
			return;
		}
	}
}

void MotorBoardManager::check() {
	for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] != NULL) {
			boards[i]->check();
		}
	}
}

void MotorBoardManager::powerOnAll() {
	for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
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
	for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
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
				for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
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
				for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
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
				// CommManager::printf("<a %d %d %d %d>", boards[0]->getLastRead(), boards[0]->getLastCurrent() , boards[0]->getTripMilliAmps(), boards[0]->getMaxMilliAmps());
				// TODO - Don't want to break JMRI. Need to fix JMRI before using the above line instead of this one:
				CommManager::printf("<a %d>", boards[0]->getLastRead());
			} else {
				for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
					if(boards[i] != NULL && strcasecmp(boards[i]->getName(), com+2) == 0) {
						// CommManager::printf("<a %s %d %d %d>", boards[i]->getName(), boards[i]->getLastRead(), boards[0]->getLastCurrent() , boards[0]->getTripMilliAmps(), boards[0]->getMaxMilliAmps());
						// When we fix JMRI's current monitor, we can use the above line instead of the below one
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
	for(uint8_t i = 0; i < MAX_MOTOR_BOARDS; i++) {
		if(boards[i] != NULL) {
			boards[i]->showStatus();
		}
	}
}
