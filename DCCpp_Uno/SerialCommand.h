/**********************************************************************

SerialCommand.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef SerialCommand_h
#define SerialCommand_h

#include "PacketRegister.h"
#include "CurrentMonitor.h"

struct SerialCommand{
  static volatile RegisterList *mRegs, *pRegs;
  static CurrentMonitor *mMonitor;
  static void init(volatile RegisterList *, volatile RegisterList *, CurrentMonitor *);
  static void parse(const char *);
}; // SerialCommand
  
#endif




