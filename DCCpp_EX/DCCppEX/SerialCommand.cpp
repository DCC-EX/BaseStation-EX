/**********************************************************************

SerialCommand.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ EX BASE STATION for the Arduino

**********************************************************************/

// DCC++ EX BASE STATION COMMUNICATES VIA THE SERIAL PORT USING SINGLE-CHARACTER TEXT COMMANDS
// WITH OPTIONAL PARAMTERS, AND BRACKETED BY < AND > SYMBOLS.  SPACES BETWEEN PARAMETERS
// ARE REQUIRED.  SPACES ANYWHERE ELSE ARE IGNORED.  A SPACE BETWEEN THE SINGLE-CHARACTER
// COMMAND AND THE FIRST PARAMETER IS ALSO NOT REQUIRED.

// See SerialCommand::parse() below for defined text commands.

#include "SerialCommand.h"
#include "DCCppEX.h"
#include "Accessories.h"
#include "Sensor.h"
#include "Outputs.h"
#include "EEStore.h"
#include "CommInterface.h"
#include "CurrentMonitor.h"

// RF24 **************************
#include "networkFunctions.h"  
#include "RF24.h"
#include "RF24Network.h"
int ID;
int THROW;
String function, option, data;
#define SYS_ID "Child Node Framework"
#define RF_VERSION "1.001.0001"  // have to rename this since DCC++ uses VERSION
// init network objects
RF24 radio(9, 10);               // nRF24L01 (CE,CSN)
//RF24 radio(10, 9);             // Alt config
RF24Network network(radio);      // Include the radio in the network
// RF24 **************************

extern int __heap_start, *__brkval;

///////////////////////////////////////////////////////////////////////////////

volatile RegisterList *SerialCommand::mRegs;
volatile RegisterList *SerialCommand::pRegs;

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::init(volatile RegisterList *_mRegs, volatile RegisterList *_pRegs){
  mRegs=_mRegs;
  pRegs=_pRegs;
  //RF24 ****************
  SPI.begin();
  network.begin(90, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS); 
  radio.begin; // // Max baud rate
  // RF24 **************
 } // SerialCommand:SerialCommand

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::parse(const char *com){

  switch(com[0]){

/***** SET ENGINE THROTTLES USING 128-STEP SPEED CONTROL ****/

    case 't':       // <t REGISTER CAB SPEED DIRECTION>
/*
 *    sets the throttle for a given register/cab combination
 *
 *    REGISTER: an internal register number, from 1 through MAX_MAIN_REGISTERS (inclusive), to store the DCC packet used to control this throttle setting
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *    SPEED: throttle speed from 0-126, or -1 for emergency stop (resets SPEED to 0)
 *    DIRECTION: 1=forward, 0=reverse.  Setting direction when speed=0 or speed=-1 only effects directionality of cab lighting for a stopped train
 *
 *    returns: <T REGISTER SPEED DIRECTION>
 *
 */
      mRegs->setThrottle(com+1);
      break;

/***** OPERATE ENGINE DECODER FUNCTIONS F0-F28 ****/

    case 'f':       // <f CAB BYTE1 [BYTE2]>
/*
 *    turns on and off engine decoder functions F0-F28 (F0 is sometimes called FL)
 *    NOTE: setting requests transmitted directly to mobile engine decoder --- current state of engine functions is not stored by this program
 *
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *
 *    To set functions F0-F4 on (=1) or off (=0):
 *
 *    BYTE1:  128 + F1*1 + F2*2 + F3*4 + F4*8 + F0*16
 *    BYTE2:  omitted
 *
 *    To set functions F5-F8 on (=1) or off (=0):
 *
 *    BYTE1:  176 + F5*1 + F6*2 + F7*4 + F8*8
 *    BYTE2:  omitted
 *
 *    To set functions F9-F12 on (=1) or off (=0):
 *
 *    BYTE1:  160 + F9*1 +F10*2 + F11*4 + F12*8
 *    BYTE2:  omitted
 *
 *    To set functions F13-F20 on (=1) or off (=0):
 *
 *    BYTE1: 222
 *    BYTE2: F13*1 + F14*2 + F15*4 + F16*8 + F17*16 + F18*32 + F19*64 + F20*128
 *
 *    To set functions F21-F28 on (=1) of off (=0):
 *
 *    BYTE1: 223
 *    BYTE2: F21*1 + F22*2 + F23*4 + F24*8 + F25*16 + F26*32 + F27*64 + F28*128
 *
 *    returns: NONE
 *
 */
      mRegs->setFunction(com+1);
      break;

/***** OPERATE STATIONARY ACCESSORY DECODERS  ****/

    case 'a':       // <a ADDRESS SUBADDRESS ACTIVATE>
/*
 *    turns an accessory (stationary) decoder on or off
 *
 *    ADDRESS:  the primary address of the decoder (0-511)
 *    SUBADDRESS: the subaddress of the decoder (0-3)
 *    ACTIVATE: 1=on (set), 0=off (clear)
 *
 *    Note that many decoders and controllers combine the ADDRESS and SUBADDRESS into a single number, N,
 *    from  1 through a max of 2044, where
 *
 *    N = (ADDRESS - 1) * 4 + SUBADDRESS + 1, for all ADDRESS>0
 *
 *    OR
 *
 *    ADDRESS = INT((N - 1) / 4) + 1  (or INT(N + 3)/4()
 *    SUBADDRESS = (N - 1) % 4        (or (N-(ADDRESS*4)) + 3;)
 *
 *    returns: NONE
 */
      mRegs->setAccessory(com+1);
      break;

/***** CREATE/EDIT/REMOVE/SHOW & OPERATE A TURN-OUT  ****/

    case 'T':       // <T ID THROW>
/*
 *   <T ID THROW>:                sets turnout ID to either the "thrown" or "unthrown" position
 *
 *   ID: the numeric ID (0-32767) of the turnout to control
 *   THROW: 0 (unthrown) or 1 (thrown)
 *
 *   returns: <H ID THROW> or <X> if turnout ID does not exist
 *
 *   *** SEE ACCESSORIES.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "T" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW TURNOUT DEFINITIONS
 */
      Turnout::parse(com+1);
// RF24 **************************
      function ="1";
      option = ID;
      data = THROW;
      sendPacket(this_node, function, option, data);
      break;
// RF24 **************************

/***** CREATE/EDIT/REMOVE/SHOW & OPERATE AN OUTPUT PIN  ****/

    case 'Z':       // <Z ID ACTIVATE>
/*
 *   <Z ID ACTIVATE>:          sets output ID to either the "active" or "inactive" state
 *
 *   ID: the numeric ID (0-32767) of the output to control
 *   ACTIVATE: 0 (active) or 1 (inactive)
 *
 *   returns: <Y ID ACTIVATE> or <X> if output ID does not exist
 *
 *   *** SEE OUTPUTS.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "O" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW TURNOUT DEFINITIONS
 */
      Output::parse(com+1);
      break;

/***** CREATE/EDIT/REMOVE/SHOW A SENSOR  ****/

    case 'S':
/*
 *   *** SEE SENSOR.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "S" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW SENSOR DEFINITIONS
 */
      Sensor::parse(com+1);
      break;

/***** SHOW STATUS OF ALL SENSORS ****/

    case 'Q':         // <Q>
/*
 *    returns: the status of each sensor ID in the form <Q ID> (active) or <q ID> (not active)
 */
      Sensor::status();
      break;

/***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/

    case 'w':      // <w CAB CV VALUE>
/*
 *    writes, without any verification, a Configuration Variable to the decoder of an engine on the main operations track
 *
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    VALUE: the value to be written to the Configuration Variable memory location (0-255)
 *
 *    returns: NONE
*/
      mRegs->writeCVByteMain(com+1);
      break;

/***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/

    case 'b':      // <b CAB CV BIT VALUE>
/*
 *    writes, without any verification, a single bit within a Configuration Variable to the decoder of an engine on the main operations track
 *
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    BIT: the bit number of the Configurarion Variable regsiter to write (0-7)
 *    VALUE: the value of the bit to be written (0-1)
 *
 *    returns: NONE
*/
      mRegs->writeCVBitMain(com+1);
      break;

/***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON PROGRAMMING TRACK  ****/

    case 'W':      // <W CV VALUE CALLBACKNUM CALLBACKSUB>
/*
 *    writes, and then verifies, a Configuration Variable to the decoder of an engine on the programming track
 *
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    VALUE: the value to be written to the Configuration Variable memory location (0-255)
 *    CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
 *    CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ EX Interface) that call this function
 *
 *    returns: <r CALLBACKNUM|CALLBACKSUB|CV Value)
 *    where VALUE is a number from 0-255 as read from the requested CV, or -1 if verificaiton read fails
*/
      pRegs->writeCVByte(com+1);
      break;

/***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON PROGRAMMING TRACK  ****/

    case 'B':      // <B CV BIT VALUE CALLBACKNUM CALLBACKSUB>
/*
 *    writes, and then verifies, a single bit within a Configuration Variable to the decoder of an engine on the programming track
 *
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    BIT: the bit number of the Configurarion Variable memory location to write (0-7)
 *    VALUE: the value of the bit to be written (0-1)
 *    CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
 *    CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ EX Interface) that call this function
 *
 *    returns: <r CALLBACKNUM|CALLBACKSUB|CV BIT VALUE)
 *    where VALUE is a number from 0-1 as read from the requested CV bit, or -1 if verificaiton read fails
*/
      pRegs->writeCVBit(com+1);
      break;

/***** READ CONFIGURATION VARIABLE BYTE FROM ENGINE DECODER ON PROGRAMMING TRACK  ****/

    case 'R':     // <R CV CALLBACKNUM CALLBACKSUB>
/*
 *    reads a Configuration Variable from the decoder of an engine on the programming track
 *
 *    CV: the number of the Configuration Variable memory location in the decoder to read from (1-1024)
 *    CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
 *    CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ EX Interface) that call this function
 *
 *    returns: <r CALLBACKNUM|CALLBACKSUB|CV VALUE)
 *    where VALUE is a number from 0-255 as read from the requested CV, or -1 if read could not be verified
*/
      pRegs->readCV(com+1);
      break;

/***** TURN ON POWER FROM MOTOR SHIELD TO TRACKS  ****/

    case '1':      // <1>
/*
 *    enables power from the motor shield to the main operations and programming tracks
 *
 *    returns: <p1>
 */
     MotorBoardManager::parse(com);
     break;

/***** TURN OFF POWER FROM MOTOR SHIELD TO TRACKS  ****/

    case '0':     // <0>
/*
 *    disables power from the motor shield to the main operations and programming tracks
 *
 *    returns: <p0>
 */
     MotorBoardManager::parse(com);
     break;

/***** READ MAIN OPERATIONS TRACK CURRENT  ****/

    case 'c':     // <c>
/*
 *    reads current being drawn on main operations track. 
 *
 *    returns <a READING CURRENT TRIPMILLIAMPS MAXMILLIAMPS>
 *
 *    READING = 0-1023 - exponentially smoothed weighted value from the raw reading on the current sense pin of the Arduino
 *    CURRENT = - current calculated from the READING and the CURRENT_CONVERSION_FACTOR for the current sense device
 *    TRIPMILLIAMPS = the value in milliamps that will trip the overcurrent shutdown of the track
 *    MAXMILLIAMPS = the maxiumum supported current of the motor board
 */
      MotorBoardManager::parse(com);
      break;

/***** READ STATUS OF DCC++ EX BASE STATION  ****/

    case 's':      // <s>
/*
 *    returns status messages containing track power status, throttle status, turn-out status, and a version number
 *    NOTE: this is very useful as a first command for an interface to send to this sketch in order to verify connectivity and update any GUI to reflect actual throttle and turn-out settings
 *
 *    returns: series of status messages that can be read by an interface to determine status of DCC++ EX Base Station and important settings
 */
      MotorBoardManager::showStatus();
      for(int i=1;i<=MAX_MAIN_REGISTERS;i++){
        if(mRegs->speedTable[i]==0)
          continue;
        CommManager::printf("<T%d %d %d>", i, mRegs->speedTable[i]>0 ? mRegs->speedTable[i] : -mRegs->speedTable[i], mRegs->speedTable[i]>0 ? 1 : 0);
      }
      CommManager::printf("<iDCC++ EX BASE STATION %s / %s: V-%s / %s %s>", ARDUINO_TYPE, MOTOR_SHIELD_NAME, VERSION, __DATE__, __TIME__);
      CommManager::showInitInfo();
      Turnout::show();
      Output::show();

      break;

/***** STORE SETTINGS IN EEPROM  ****/

    case 'E':     // <E>
/*
 *    stores settings for turnouts and sensors EEPROM
 *
 *    returns: <e nTurnouts nSensors>
*/

    EEStore::store();
    CommManager::printf("<e %d %d %d>", EEStore::eeStore->data.nTurnouts, EEStore::eeStore->data.nSensors, EEStore::eeStore->data.nOutputs);
    break;

/***** CLEAR SETTINGS IN EEPROM  ****/

    case 'e':     // <e>
/*
 *    clears settings for Turnouts in EEPROM
 *
 *    returns: <O>
*/

    EEStore::clear();
    CommManager::printf("<O>");
    break;

/***** PRINT CARRIAGE RETURN IN SERIAL MONITOR WINDOW  ****/

    case ' ':     // < >
/*
 *    simply prints a carriage return - useful when interacting with Ardiuno through serial monitor window
 *
 *    returns: a carriage return
*/
      CommManager::printf("\n");
      break;

///
/// THE FOLLOWING COMMANDS ARE NOT NEEDED FOR NORMAL OPERATIONS AND ARE ONLY USED FOR TESTING AND DEBUGGING PURPOSES
/// PLEASE SEE SPECIFIC WARNINGS IN EACH COMMAND BELOW
///

/***** ENTER DIAGNOSTIC MODE  ****/

    case 'D':       // <D>
/*
 *    changes the clock speed of the chip and the pre-scaler for the timers so that you can visually see the DCC signals flickering with an LED
 *    SERIAL COMMUNICAITON WILL BE INTERUPTED ONCE THIS COMMAND IS ISSUED - MUST RESET BOARD OR RE-OPEN SERIAL WINDOW TO RE-ESTABLISH COMMS
 */

    Serial.println(F("\nEntering Diagnostic Mode..."));
    delay(1000);

    bitClear(TCCR1B,CS12);    // set Timer 1 prescale=8 - SLOWS NORMAL SPEED BY FACTOR OF 8
    bitSet(TCCR1B,CS11);
    bitClear(TCCR1B,CS10);

    #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)     // Configuration for UNO or Nano

      bitSet(TCCR0B,CS02);    // set Timer 0 prescale=256 - SLOWS NORMAL SPEED BY A FACTOR OF 4
      bitClear(TCCR0B,CS01);
      bitClear(TCCR0B,CS00);

    #else                     // Configuration for MEGA

      bitClear(TCCR3B,CS32);    // set Timer 3 prescale=8 - SLOWS NORMAL SPEED BY A FACTOR OF 8
      bitSet(TCCR3B,CS31);
      bitClear(TCCR3B,CS30);

    #endif

    CLKPR=0x80;           // THIS SLOWS DOWN SYSYEM CLOCK BY FACTOR OF 256
    CLKPR=0x08;           // BOARD MUST BE RESET TO RESUME NORMAL OPERATIONS

    break;

/***** WRITE A DCC PACKET TO ONE OF THE REGSITERS DRIVING THE MAIN OPERATIONS TRACK  ****/

    case 'M':       // <M REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
/*
 *   writes a DCC packet of two, three, four, or five hexidecimal bytes to a register driving the main operations track
 *   FOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
 *
 *    REGISTER: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTER=0) or write and store (if REGISTER>0) the packet
 *    BYTE1:  first hexidecimal byte in the packet
 *    BYTE2:  second hexidecimal byte in the packet
 *    BYTE3:  optional third hexidecimal byte in the packet
 *    BYTE4:  optional fourth hexidecimal byte in the packet
 *    BYTE5:  optional fifth hexidecimal byte in the packet
 *
 *    returns: NONE
 */
      mRegs->writeTextPacket(com+1);
      break;

/***** WRITE A DCC PACKET TO ONE OF THE REGSITERS DRIVING THE MAIN OPERATIONS TRACK  ****/

    case 'P':       // <P REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
/*
 *   writes a DCC packet of two, three, four, or five hexidecimal bytes to a register driving the programming track
 *   FOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
 *
 *    REGISTER: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTER=0) or write and store (if REGISTER>0) the packet
 *    BYTE1:  first hexidecimal byte in the packet
 *    BYTE2:  second hexidecimal byte in the packet
 *    BYTE3:  optional third hexidecimal byte in the packet
 *    BYTE4:  optional fourth hexidecimal byte in the packet
 *    BYTE5:  optional fifth hexidecimal byte in the packet
 *
 *    returns: NONE
 */
      pRegs->writeTextPacket(com+1);
      break;

/***** ATTEMPTS TO DETERMINE HOW MUCH FREE SRAM IS AVAILABLE IN ARDUINO  ****/

    case 'F':     // <F>
/*
 *     measure amount of free SRAM memory left on the Arduino based on trick found on the internet.
 *     Useful when setting dynamic array sizes, considering the Uno only has 2048 bytes of dynamic SRAM.
 *     Unfortunately not very reliable --- would be great to find a better method
 *
 *     returns: <f MEM>
 *     where MEM is the number of free bytes remaining in the Arduino's SRAM
 */
      int v;
      CommManager::printf("<f%d>", (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
      break;

/***** LISTS BIT CONTENTS OF ALL INTERNAL DCC PACKET REGISTERS  ****/

    case 'L':     // <L>
/*
 *    lists the packet contents of the main operations track registers and the programming track registers
 *    FOR DIAGNOSTIC AND TESTING USE ONLY
 */
      CommManager::printf("\n");
      for(Register *p=mRegs->reg;p<=mRegs->maxLoadedReg;p++){
        CommManager::printf("M%d:\t%d\t%d\t%d\t", (int)(p-mRegs->reg), (int)p, (int)p->activePacket, p->activePacket->nBits);
        for(int i=0;i<10;i++){
          CommManager::printf("%02x\t", p->activePacket->buf[i]);
        }
        CommManager::printf("\n");
      }
      for(Register *p=pRegs->reg;p<=pRegs->maxLoadedReg;p++){
        CommManager::printf("P%d:\t%d\t%d\t%d\t", (int)(p-pRegs->reg), (int)p, (int)p->activePacket, p->activePacket->nBits);
        for(int i=0;i<10;i++){
          CommManager::printf("%02x\t", p->activePacket->buf[i]);
        }
        CommManager::printf("\n");
      }
      CommManager::printf("\n");
      break;

    case 'p':     // <p>
/*
 * Lists all registered motor boards with current status.
 */
      MotorBoardManager::showStatus();
      break;
  } // switch
}; // SerialCommand::parse

///////////////////////////////////////////////////////////////////////////////
