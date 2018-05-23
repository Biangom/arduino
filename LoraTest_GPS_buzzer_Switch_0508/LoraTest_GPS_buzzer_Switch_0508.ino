#include "SkLoRa.h"
#include <TimerOne.h>

SkLoRa sk;
bool b_gps = false;
bool b_switch = false;
int count = 0;
void setup()  
{
  Serial.begin(19200);  // Computer - Arduino
  Serial1.begin(19200); // Arduino - LoRa Module
  Serial2.begin(9600);  // Arduino - GPS Module
 
  Timer1.initialize(1000000000); // 10초
  Timer1.attachInterrupt(gps_interrupt);
  
  Serial.println("Connection Start");
  pinMode(2,INPUT_PULLUP);
  sk.reset();
 }
 
void loop() // run over and over
{
  String line;
  char ctl = '\0';

  if (Serial1.available()) {
    line = Serial1.readStringUntil('\n');
    //DevReset 시 Reset
    if(line.startsWith("DevReset")){
      delay(5000);
      sk.reset();
    }
    //Join 완료 시
    if(line.startsWith("join Complted")){
      sk.b_join = true;
      Serial.println(sk.b_join);
    }
   /*
    //Emergency 신호 수신 시 ( 16진수값 )
    if(line.startsWith("TotalTxTime")){
      sk.buzzer(9,10);
      
    }*/
    Serial.println(line);
  }
 
  if(Serial.available()){
     char i = Serial.read();
     short cli = i;
     Serial1.write(cli);
  }
 //Open House에서 Test 할 용도
 if(b_gps && sk.b_join){
  //(b_gps){
    sk.transmission_gps();
    b_gps = false;
  }
  /*
  부저 함수화 해야함
  //if(sk.b_buzzer && sk.b_join){
    if((sk.b_buzzer){
      //tone(PIN, 1000); // 5옥타브 도
      sk.b_buzzer = false;
  }*/
  
  //스위치
   if(sk.switchon(2) == LOW && count == 0){  
      sk.transmission_switch();
      count = 1;
      
   }
  // Serial.print(count);
   
  
}

void gps_interrupt(){
  b_gps = true;
  count = 0;
}





