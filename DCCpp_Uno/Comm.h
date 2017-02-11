

/**********************************************************************

Comm.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "Config.h"

#if COMM_TYPE == 1                 // Ethernet Shield Card Selected

  #if COMM_INTERFACE == 1
    #define COMM_SHIELD_NAME      "ARDUINO-CC ETHERNET SHIELD (WIZNET 5100)"
    #include <Ethernet.h>         // built-in Arduino.cc library
    extern EthernetServer INTERFACE;
    #define NETWORK_CLIENT EthernetClient

  #elif COMM_INTERFACE == 2
    #define COMM_SHIELD_NAME      "ARDUINO-ORG ETHERNET-2 SHIELD (WIZNET 5500)"
    #include <Ethernet2.h>        // https://github.com/arduino-org/Arduino
    extern EthernetServer INTERFACE;
    #define NETWORK_CLIENT EthernetClient

  #elif COMM_INTERFACE == 3
    #define COMM_SHIELD_NAME      "SEEED STUDIO ETHERNET SHIELD (WIZNET 5200)"
    #include <EthernetV2_0.h>     // https://github.com/Seeed-Studio/Ethernet_Shield_W5200
    extern EthernetServer INTERFACE;
    #define NETWORK_CLIENT EthernetClient

  #elif COMM_INTERFACE == 4
    #define COMM_SHIELD_NAME      "ESP8266 Wifi (CustomFirmware)"
    #include <IPAddress.h>
#ifdef HAVE_HWSERIAL1
    #define WIFI_SERIAL_LINK Serial1
    extern HardwareSerial Serial1;
#else
    #define WIFI_SERIAL_LINK wifiSerial
    extern SoftwareSerial wifiSerial;
#endif
    #define NETWORK_CLIENT WiFiEspClient

  #endif
  extern IPAddress localAddress;

#endif




