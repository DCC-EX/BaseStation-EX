## PLEASE NOTE: All future development is on the CommandStation-EX repository. This is maintained here for historical reasons

# What's DCC++ EX
------------

DCC++ EX is an open-source hardware and software system for the operation of DCC-equipped model railroads. It expands on the work of Gregg E. Berman's original DCC++ (which can be found here in the BaseStation-Classic repository)

The system consists of two parts, the DCC++ EX Base Station and the DCC++ Controller.

The DCC++ EX Base Station consists of an Arduino micro controller fitted with an Arduino Motor Shield that can be connected directly to the tracks of a model railroad.

The DCC++ Controller provides operators with a customizable GUI to control their model railroad.  It is written in Java using the Processing graphics library and IDE and communicates with the DCC++ EX Base Station via a standard serial connection over a USB cable or wireless over BlueTooth.

# What’s in this Repository
-------------------------

This repository, BaseStation-EX, contains a complete DCC++ EX Base Station sketch designed for compiling and uploading into an Arduino Uno, Mega, or Nano.  All sketch files are in the folder named DCCppEX. More information about the sketch can be found in the included PDF file.

To utilize this sketch, simply download a zip file of this repository and open the file DCCppEX.ino within the DCCppEX folder using your Arduino IDE.  Please do not rename the folder containing the sketch code, nor add any files to that folder.  The Arduino IDE relies on the structure and name of the folder to properly display and compile the code.

The latest production release of the Master branch is 2.0.1:

* Supports the Arduino Uno, Arduino Mega, and Arduino Nano
* Built-in configuration for both the original Arduino Motor Shield, Pololu MC33926 Motor Shield, LMD18200, and BTS7960B
* Built-in configuration and support of Ethernet Shields (for use with Mega only) and the ESP82266 WiFi module.

For more information on the overall DCC++ EX system, please follow the links in the PDF file.

Detailed diagrams showing pin mappings and required jumpers for the Motor Shields can be found in the Documentation Repository

The Master branch contains all of the Base Station functionality showed in the DCC++ YouTube channel with the exception of 2 layout-specific modules:

* Control for an RGB LED Light Strip using pins 44, 45, and 46 on the Mega
* An embedded AutoPilot routine that randomly selects a train to run through the entire layout, after which it is brought back into its original siding and the the patterns repeats with another randomly-selected train.  This is the AutoPilot routine showed on the DCC++ YouTube channel.  It does not require any computer, nor DCC++ Controller to be running (DCC++ Controller contains a much more complicated 3-train Auto Pilot mode, also as shown on the DCC++ YouTube channel).

Since these modules are very layout-specififc, they are not included in the Master branch.  However, they are included in the Development branch.  Please feel free to download and copy any relevant code to customize your own version of DCC++ Base Station.

# How to find more information
--------------------------

[DCC++ EX WEB Page](https://dcc-ex.github.io "DCC++ EX WEB Page")

[The DCC++ EX Discord and live support](https://discord.gg/y2sB4Fp "The DCC++ EX Discord Server")

[TrainBoard DCC++ Forum](https://www.trainboard.com/highball/index.php?forums/dcc.177/ "TrainBoard DCC++ Forum")

-May 2020

