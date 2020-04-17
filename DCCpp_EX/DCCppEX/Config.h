/**********************************************************************

Config.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

/////////////////////////////////////////////////////////////////////////////////////
//  NOTE: Before connecting these boards and selecting one in this software
//        check the quick install guides!!! Some of these boards require a voltage
//        generating resitor on the current sense pin of the device. Failure to select
//        the correct resistor could damage the sense pin on your Arduino or destroy
//        the device.
//
// DEFINE MOTOR_SHIELD_TYPE ACCORDING TO THE FOLLOWING TABLE:
//
//  0 = ARDUINO MOTOR SHIELD            (MAX 18V/2A  PER CHANNEL)  Arduino Motor shield Rev3 based on the L298
//  1 = POLOLU MC33926 MOTOR SHIELD     (MAX 28V/2.5 PER CHANNEL)  Pololu MC33926 Motor Driver (shield or carrier)
//  2 = BTS7960B_5A                     (MAX 27V/5A  PER CHANNEL)  Infineon Technologies BTS 7960 Motor Driver Module. Max Output 5A (43A actual max)
//  3 = BTS7906B_10A                    (MAX 27V/10A PER CHANNEL)  Infineon Technologies BTS 7960 Motor Driver Module. Max Output 10A (43A actual max)
//  4 = LMD18200 MOTOR DRIVER MODULE    (MAX 55V/3A  PER CHANNEL)  LMD18200 Motor Driver Board (6A Max actual instantaneous peak)
//  5 = MAX 471 CURRENT SENSE MODULE    (MAX 28V/3A  PER CHANNEL)  MAX 471 connected to an LMD18200 Motor Driver

#define MOTOR_SHIELD_TYPE   0

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE NUMBER OF MAIN TRACK REGISTER

#define MAX_MAIN_REGISTERS 12

/////////////////////////////////////////////////////////////////////////////////////
//
// NOTE: Only the Mega currently supports networking since pin 4 is in use by DCC++
//       and creates a conflict with the Ethernet Shield's need for pin 4. For the Mega,
//       DCC++ uses pin 2 instead of pin 4. This could be changed in the future but
//       could cause issues with people with old UNO jumper settings applying an update
//
// DEFINE COMMUNICATIONS INTERFACE
//
//  0 = Built-in Serial Port
//  1 = Arduino.cc Ethernet/SD-Card Shield
//  2 = Arduino.org Ethernet/SD-Card Shield
//  3 = Seeed Studio Ethernet/SD-Card Shield W5200
//  4 = ESP8266 WiFi module

#define COMM_INTERFACE   0

#if COMM_INTERFACE == 4
	/////////////////////////////////////////////////////////////////////////////////////
	//
	// DEFINE WiFi Parameters
	//
	#define WIFI_SSID ""
	#define WIFI_PASSWORD ""

	// The WIFI_SERIAL_RX and WIFI_SERIAL_TX config values are not used on the Mega2650 which
	// has multiple Serial interfaces.  For Mega2560 the default is Serial1.
	//#define WIFI_SERIAL_RX
	//#define WIFI_SERIAL_TX

	// For the Uno you can also use Serial instead of SoftwareSerial by defining the following
	// option.  Enabling this option will set the IO device as Serial instead of
	// SoftwareSerial using the above WIFI_SERIAL_RX and WIFI_SERIAL_TX pin assignments. This
	// can also be used on the Mega2560 to allow Serial1 to be used for other purposes.
	// #define USE_SERIAL_FOR_WIFI

	// This defines the speed at which the Arduino will communicate with the ESP8266 module.
	// When using the ESP8266 on an Uno it is recommended to use 9600, for Mega2560 the
	// higher speed can be used.  Set this based on your ESP8266 module configuration.
	// Common defaults are 9600 or 115200.
	#define WIFI_SERIAL_LINK_SPEED 115200
#endif

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE STATIC IP ADDRESS *OR* COMMENT OUT TO USE DHCP
//

//#define IP_ADDRESS { 192, 168, 1, 200 }

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE PORT TO USE FOR ETHERNET COMMUNICATIONS INTERFACE
//

#define ETHERNET_PORT 2560

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE MAC ADDRESS ARRAY FOR ETHERNET COMMUNICATIONS INTERFACE
//
// Note: This is not used with ESP8266 WiFi modules.

#define MAC_ADDRESS {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }

/////////////////////////////////////////////////////////////////////////////////////
//
// USE_TRIGGERPIN: Enable code that switches the trigger pin on and off at end
//                 of the preamble. This takes some clock cycles in the
//                 interrupt routine for the main track.
// USE_TRIGGERPIN_PER_BIT: As above but for every bit. This only makes sense
//                 if USE_TRIGGERPIN is set.
//
// The value of the TRIGGERPIN is defined in DCCppEX.h because it might
// be board specific
//
//#define USE_TRIGGERPIN
//#define USE_TRIGGERPIN_PER_BIT

/////////////////////////////////////////////////////////////////////////////////////
//
// Define only of you need the store to EEPROM feature. This takes RAM and
// you may need to use less MAX_MAIN_REGISTERS to compensate (at least on the UNO)
//#define EESTORE

/////////////////////////////////////////////////////////////////////////////////////
//
// This shows the status and version at startup. This takes RAM. You can comment
// this line if you need to increase MAX_MAIN_REGISTERS(at least on the UNO)
#define SHOWCONFIG

/////////////////////////////////////////////////////////////////////////////////////
//
// PREAMBLE_MAIN: Length of the preamble on the main track. Per standard this should
//                be at least 14 bits but if some equipment wants to insert a RailCom
//                cutout this should be at least 16 bits.
// PERAMBLE_PROG: Length of the preamble on the programming track. Per standard this
//                should be at least 22 bits 
//
#define PREAMBLE_MAIN 16
#define PREAMBLE_PROG 22

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE LCD SCREEN USAGE BY THE BASE STATION
//
// Note: This feature requires an I2C enabled LCD screen using a PCF8574 based chipset.
//
// #define ENABLE_LCD

#ifdef ENABLE_LCD
	// This defines the I2C address for the LCD device
	#define LCD_ADDRESS 0x3F

	// This defines the number of columns the LCD device has
	#define LCD_COLUMNS 16

	// This defines the number of lines the LCD device has
	#define LCD_LINES 2
#endif
