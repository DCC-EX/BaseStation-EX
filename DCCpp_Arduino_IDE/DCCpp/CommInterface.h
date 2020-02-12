/*
 * CommInterface.h
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 */

#ifndef COMMINTERFACE_H_
#define COMMINTERFACE_H_

class CommInterface {
public:
	virtual void process() = 0;
	virtual void showConfiguration() = 0;
	virtual void showInitInfo() = 0;
	virtual void send(const char *buf) = 0;
	virtual ~CommInterface();
};

class CommManager {
public:
	static void update();
	static void registerInterface(CommInterface *interface);
	static void showConfiguration();
	static void showInitInfo();
	static void printf(const char *fmt, ...);
private:
	static CommInterface *interfaces[10];
	static int nextInterface;
};

#endif /* COMMINTERFACE_H_ */
