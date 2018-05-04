#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3); // RX, TX
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200); // Computer - Arduino
  mySerial.begin(19200);// Arduino - LoRa
  Serial.println("Connection Start");
}

void loop() {
  // put your main code here, to run repeatedly:
  String line;

   //LoRa 모듈에서 보낸 메세지가 있을 경우
  if (mySerial.available()) {
    line = mySerial.readStringUntil('\n');
    if(line.startsWith("DevReset")){
      delay(5000);

    }
    Serial.println(line);
  }
  if(Serial.available()){
     char i = Serial.read();
     short n1 = i;
   
     mySerial.write(n1);
   
  } 



}




