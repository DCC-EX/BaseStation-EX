/*
 * CommInterfaceSerial.h
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#ifndef COMMINTERFACESERIAL_H_
#define COMMINTERFACESERIAL_H_

#include "CommInterface.h"
#include <HardwareSerial.h>

class HardwareSerialInterface : public CommInterface {
public:
	HardwareSerialInterface(HardwareSerial &serial, long baud=115200);
	void process();
	void showConfiguration();
	void showInitInfo();
	void send(const char *buf);
protected:
	Stream &serialStream;
	long baud;
	String buffer;
	bool inCommandPayload;
};

#endif /* COMMINTERFACESERIAL_H_ */
