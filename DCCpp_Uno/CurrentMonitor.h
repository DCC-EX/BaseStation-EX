/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

#define  CURRENT_SAMPLE_SMOOTHING          0.01

// Arduino motor board: 890mA == 300*0.0049/1.65
#define  CURRENT_SAMPLE_MAX_ARDUINO        300
// Pololu motor board: 1.493A == 160*0.0049/0.525
#define  CURRENT_SAMPLE_MAX_POLOLU         160
// BTS7960B motor board: 5.133A == 11*0.0049/0.0105
#define  CURRENT_SAMPLE_MAX_BTS7960B_5A    11
// BTS7960B motor board: 10.266A == 22*0.0049/0.0105
#define  CURRENT_SAMPLE_MAX_BTS7960B_10A   22

#if MOTOR_SHIELD_TYPE == 0
  #define CURRENT_SAMPLE_MAX CURRENT_SAMPLE_MAX_ARDUINO
#elif MOTOR_SHIELD_TYPE == 1
  #define CURRENT_SAMPLE_MAX CURRENT_SAMPLE_MAX_POLOLU
#elif MOTOR_SHIELD_TYPE == 2
  #define CURRENT_SAMPLE_MAX CURRENT_SAMPLE_MAX_BTS7960B_5A
#endif

#ifdef ARDUINO_AVR_UNO                        // Configuration for UNO
  #define  CURRENT_SAMPLE_TIME        10
#else                                         // Configuration for MEGA
  #define  CURRENT_SAMPLE_TIME        1
#endif

struct CurrentMonitor{
  static long int sampleTime;
  int sensePin;
  int enablePin;
  float current;
  char *msg;
  bool triggered;
  CurrentMonitor(int, int, char *);
  static boolean checkTime();
  void check();
};

#endif

