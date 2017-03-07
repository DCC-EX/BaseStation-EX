/*
 * CommInterfaceESP.h
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#ifndef COMMINTERFACEESP_H_
#define COMMINTERFACEESP_H_

#include "CommInterface.h"
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <IPAddress.h>

class ESPInterface : public CommInterface {
public:
	ESPInterface(Stream &serialStream, IPAddress *ip=NULL);
	void process();
	void showConfiguration();
	void showInitInfo();
	void send(const char *buf);
private:
	void init(IPAddress *ip = NULL);
	Stream &_serialStream;
	IPAddress _localAddress;
	String _buffer;
	bool _inCommandPayload;
};

class ESPHardwareSerialInterface : public ESPInterface {
public:
	ESPHardwareSerialInterface(HardwareSerial &serial, long baud=115200, IPAddress *ip = NULL);
	void showConfiguration();
private:
	Stream &initSerialStream(HardwareSerial &serial, long baud);
	long _baud;
	HardwareSerial &_serial;
};

class ESPSoftwareSerialInterface : public ESPInterface {
public:
	ESPSoftwareSerialInterface(int rxPin, int txPin, long baud=115200, IPAddress *ip = NULL);
	void showConfiguration();
private:
	Stream &initSerialStream(int rxPin, int txPin, long baud);
	SoftwareSerial *_serial;
	long _baud;
	int _rxPin;
	int _txPin;
};

#endif /* COMMINTERFACEESP_H_ */
