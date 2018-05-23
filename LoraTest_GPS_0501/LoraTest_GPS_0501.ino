#include "SkLoRa.h"
#include <TimerOne.h>

SkLoRa sk;
bool b_gps = false;

void setup()  
{
  Serial.begin(19200);  // Computer - Arduino
  Serial1.begin(19200); // Arduino - LoRa Module
  Serial2.begin(9600);  // Arduino - GPS Module
 
  Timer1.initialize(1000000000); // 10초
  Timer1.attachInterrupt(gps_interrupt);
  
  Serial.println("Connection Start");
  sk.reset();
 }
 
void loop() // run over and over
{
  String line;
  char ctl = '\0';

  if (Serial1.available()) {
    line = Serial1.readStringUntil('\n');
    if(line.startsWith("DevReset")){
      delay(5000);
      sk.reset();
    }
    if(line.startsWith("join Complted")){
      sk.b_join = true;
      Serial.println(sk.b_join);
    }
    Serial.println(line);
  }
 
  if(Serial.available()){
 
    switch(Serial.read()){
      case 'r' : 
        sk.reset();
        ctl = '\0';
        break;
      case 'l':
        sk.setLinkCheckRequest(); 
        break;
      case 'c':
        sk.check_connection();
        break;
      case 'v':
        sk.getVersion();   
        break;
      case 't':
        sk.setTimeSyncRequest();
        break;
      default :
        break;
    }
  }
 //Open House에서 Test 할 용도
 //if(b_gps||sk.b_join){
  if(b_gps){
    sk.transmission_gps();
    b_gps = false;
  }
}

void gps_interrupt(){
  b_gps = true;
}





