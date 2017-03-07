/*
 * CommInterfaceEthernet.h
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#ifndef COMMINTERFACEETHERNET_H_
#define COMMINTERFACEETHERNET_H_

#include "CommInterface.h"

#define SDCARD_CS 4

#if COMM_INTERFACE == 1
  #include <Ethernet.h>         // built-in Arduino.cc library
#elif COMM_INTERFACE == 2
  #include <Ethernet2.h>        // https://github.com/arduino-org/Arduino
#elif COMM_INTERFACE == 3
  #include <EthernetV2_0.h>     // https://github.com/Seeed-Studio/Ethernet_Shield_W5200
#endif

#if COMM_INTERFACE >= 1 && COMM_INTERFACE <= 3
class EthernetInterface : public CommInterface {
public:
	EthernetInterface();
	void process();
	void showConfiguration();
	void showInitInfo();
	void send(const char *buf);
protected:
	EthernetServer server;
	byte mac[];
	String buffer;
	bool inCommandPayload;
};
#endif

#endif /* COMMINTERFACESERIAL_H_ */
