/**********************************************************************

SerialCommand.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ EX BASE STATION for the Arduino

**********************************************************************/

#ifndef SerialCommand_h
#define SerialCommand_h

#include "PacketRegister.h"

// RF24 ********************************
const uint16_t this_node = 05;   // Node address referenced in networkFunctions.h
// RF24 ********************************

struct SerialCommand{
  static volatile RegisterList *mRegs, *pRegs;
  static void init(volatile RegisterList *, volatile RegisterList *);
  static void parse(const char *);
}; // SerialCommand

#endif




