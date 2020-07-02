/**********************************************************************

DCCppEX.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ EX BASE STATION for the Arduino

**********************************************************************/

#include "Config.h"

// use an include guard
#ifndef DCCppEX_h
#define DCCppEX_h

/////////////////////////////////////////////////////////////////////////////////////
// RELEASE VERSION
/////////////////////////////////////////////////////////////////////////////////////

#define VERSION "2.1.2"


/////////////////////////////////////////////////////////////////////////////////////
// AUTO-SELECT ARDUINO BOARD
/////////////////////////////////////////////////////////////////////////////////////

#ifdef ARDUINO_AVR_MEGA                   // is using Mega 1280, define as Mega 2560 (pinouts and functionality are identical)
  #define ARDUINO_AVR_MEGA2560  
#endif

#if defined ARDUINO_AVR_UNO

  #define ARDUINO_TYPE    "UNO"

  #define DCC_SIGNAL_PIN_MAIN 10          // Ardunio Uno  - uses OC1B
  #define DCC_SIGNAL_PIN_PROG 5           // Arduino Uno  - uses OC0B

  #if COMM_INTERFACE != 0                 // Serial was not selected

    #error CANNOT COMPILE - DCC++ EX FOR THE UNO CAN ONLY USE SERIAL COMMUNICATION - PLEASE SELECT THIS IN THE CONFIG FILE

  #endif

#elif defined ARDUINO_AVR_NANO

  #define ARDUINO_TYPE    "NANO"

  #define DCC_SIGNAL_PIN_MAIN 10          // Ardunio Nano  - uses OC1B
  #define DCC_SIGNAL_PIN_PROG 5           // Arduino Nano  - uses OC0B

  #if COMM_INTERFACE != 0                 // Serial was not selected

    #error CANNOT COMPILE - DCC++ EX FOR THE UNO CAN ONLY USE SERIAL COMMUNICATION - PLEASE SELECT THIS IN THE CONFIG FILE

  #endif

#elif defined  ARDUINO_AVR_MEGA2560

  #define ARDUINO_TYPE    "MEGA"

  #define DCC_SIGNAL_PIN_MAIN 12          // Arduino Mega - uses OC1B
  #define DCC_SIGNAL_PIN_PROG 2           // Arduino Mega - uses OC3B

#else

  #error CANNOT COMPILE - DCC++ EX ONLY WORKS WITH AN ARDUINO UNO, NANO 328, OR ARDUINO MEGA 1280/2560

#endif

////////////////////////////////////////////////////////////////////////////////////
//  Set trigger pin for data analysis if enabled.
////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_TRIGGERPIN
#define TRIGGERPIN 6                     // Should work on UNO and MEGA
#endif


/////////////////////////////////////////////////////////////////////////////////////
// SELECT MOTOR SHIELD       current in milliamps computed from ((Vcc/1024)/volts_per_amp) * 1000
//                           VCC is the voltage range of the Arduino sensor pin (5v) while 1024
//                           is the resolution or the range of the sensor (10bits). volts_per_amp
//                           is the number of volts out of the current sensor board that corresponds
//                           to one amp of current.
/////////////////////////////////////////////////////////////////////////////////////

#if MOTOR_SHIELD_TYPE == 0

  #define MOTOR_SHIELD_NAME "ARDUINO MOTOR SHIELD"

  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  // ((5/1024)/1.65) * 1000
  #define CURRENT_CONVERSION_FACTOR 2.96 // 2.96 * 100 so we can do integer math later

#elif MOTOR_SHIELD_TYPE == 1

  #define MOTOR_SHIELD_NAME "POLOLU MC33926 MOTOR SHIELD"

  #define SIGNAL_ENABLE_PIN_MAIN 9
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 7
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 8

  // (5/1024) /.525 * 1000
  #define CURRENT_CONVERSION_FACTOR 9.30 // 9.30* 100 so we can do integer math later

#elif MOTOR_SHIELD_TYPE == 2

  #define MOTOR_SHIELD_NAME "BTS7960B BASED MOTOR SHIELD 5A"

  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  #define CURRENT_CONVERSION_FACTOR 465.00 // TODO verify this value

  #elif MOTOR_SHIELD_TYPE == 3

  #define MOTOR_SHIELD_NAME "BTS7960B BASED MOTOR SHIELD 10A"

  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  // (5/1024) /.0105 * 1000
  #define CURRENT_CONVERSION_FACTOR 465.00  // 456 * 100 so we can do integer math later 

   #elif MOTOR_SHIELD_TYPE == 4
   // uses current sense resister of 2.2 kOhms between pin 8 and ground.
   // The LMD18200 delivers 377uA per Amp of output current and will report
   // current over its rating of 3A up to 6A! We have to take this into consideration
   // so as not to damage the Arduino
   // R = Vout/Imax = 5/(.000377/6) = 2200 Ohms
   // So 1A would send .83V to the Arduino sense pin.
   // That means with a 2200 Ohm resistor to a 5V arduinio pin, that is .83V / Amp

  #define MOTOR_SHIELD_NAME "LMD18200" //Onboard current sense
  
  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  // (5/1024) /.83* 1000
  #define CURRENT_CONVERSION_FACTOR 5.88  // 5.88 * 100 so we can do integer math later

  #elif MOTOR_SHIELD_TYPE == 5

  #define MOTOR_SHIELD_NAME "LMD18200/MAX471" // Using MAX471 for current sense

  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  // (5/1024) /1 * 1000
  #define CURRENT_CONVERSION_FACTOR 4.88  // 4.88 * 100 so we can do integer math later

#else

  #error CANNOT COMPILE - PLEASE SELECT A PROPER MOTOR SHIELD TYPE

#endif

/////////////////////////////////////////////////////////////////////////////////////
// SELECT COMMUNICATION INTERACE
/////////////////////////////////////////////////////////////////////////////////////

#if !defined(COMM_INTERFACE) || COMM_INTERFACE < 0 || COMM_INTERFACE > 4
  #error CANNOT COMPILE - Please select a proper value for COMM_INTERFACE in CONFIG.H file
#endif

/////////////////////////////////////////////////////////////////////////////////////
// SET WHETHER TO SHOW PACKETS - DIAGNOSTIC MODE ONLY
/////////////////////////////////////////////////////////////////////////////////////

// If SHOW_PACKETS is set to 1, then for select main operations track commands that modify an internal DCC packet register,
// if printFlag for that command is also set to 1, DCC++ EX BASE STATION will additionally return the 
// DCC packet contents of the modified register in the following format:

//    <* REG: B1 B2 ... Bn CSUM / REPEAT>
//
//    REG: the number of the main operations track packet register that was modified
//    B1: the first hexidecimal byte of the DCC packet
//    B2: the second hexidecimal byte of the DCC packet
//    Bn: the nth hexidecimal byte of the DCC packet
//    CSUM: a checksum byte that is required to be the final byte in any DCC packet
//    REPEAT: the number of times the DCC packet was re-transmitted to the tracks after its iniital transmission
 
#define SHOW_PACKETS  0       // set to zero to disable printing of every packet for select main operations track commands

/////////////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_LCD
#include <Wire.h>
  #if defined LIB_TYPE_PCF8574
    #include <LiquidCrystal_PCF8574.h>
    extern LiquidCrystal_PCF8574 lcdDisplay;
  #elif defined LIB_TYPE_I2C
    #include <LiquidCrystal_I2C.h>
    extern LiquidCrystal_I2C lcdDisplay;
  #endif
extern bool lcdEnabled;
#endif

#endif