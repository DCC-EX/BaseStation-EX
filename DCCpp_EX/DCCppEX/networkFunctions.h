/**************************************************************************
 * 
 * networkFunctions.h
 * @version 1.002
 * Supports Serial and nRF2401 Radio Communicataions
 * 
 */
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

typedef struct PKT_DEF {
  String function; // String is part of RF24Network
  String option;
  String data;
  uint16_t source_node;
};
#define PACKET_MAX_SIZE 16
struct PKT_DEF parsePKT(String packet) {
 String delimiter = F("/"); // Packet delimiter character
 PKT_DEF pkt;
 int segment = 1;
 int delimIndex = packet.indexOf(delimiter);
 if(delimIndex == 0) { // drop leading delimiter, if any
  packet.remove(0,1);
  delimIndex = packet.indexOf(delimiter);
 }
 int lastDelim = -1;
 while(delimIndex >= 0) {
   switch(segment){
     case 1:
       pkt.function = packet.substring(lastDelim + 1, delimIndex);
 
       break;
     case 2:
       pkt.option = packet.substring(lastDelim + 1, delimIndex);
       break;
     case 3:
       pkt.data = packet.substring(lastDelim + 1, delimIndex);
       break;
     default:      
       break;
   }
   segment++;
   lastDelim = delimIndex;
   delimIndex = packet.indexOf(delimiter, lastDelim + 1);  
 }
 // if we don't already have a data field  
 // any trailing element without a deliminter is data
 if(pkt.data.length() == 0 && lastDelim < packet.length()){ 
   pkt.data = packet.substring(lastDelim + 1);
 }
 return pkt;
}
PKT_DEF pollNet(){
 
  //*******************
 extern RF24Network network;
  char packetBuffer[PACKET_MAX_SIZE + 1] = ""; // initialize buffer to hold packet
  bool pkt_received = false;
  PKT_DEF pkt;
  network.update(); // Process network data
  while( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    network.read(header, &packetBuffer, PACKET_MAX_SIZE); // Read the incoming data
    pkt = parsePKT(String(packetBuffer));
    pkt.source_node = header.from_node;
    pkt_received = true;
  } 
  if( !pkt_received && Serial.available() ) {
    int count = Serial.readBytes(packetBuffer, PACKET_MAX_SIZE);
    if(count){
      pkt = parsePKT(String(packetBuffer));
      pkt.source_node = this_node;
      pkt_received = true;
    }
  }
  if(!pkt_received){
    pkt.function = "0";
  }
  return pkt;
 }
void sendMessage(uint16_t to_node, char *data, int bytes){
//****************************
 extern RF24Network network;
  RF24NetworkHeader header(to_node);
  network.write(header, data, bytes); // Send the data
 }
void sendPacket(uint16_t to_node, String function, String option, String data){
  String delimiter = F("/"); // Packet delimiter character
  String message = delimiter + function + delimiter + option + delimiter + data;
  sendMessage(to_node, message.c_str(), message.length());
}
