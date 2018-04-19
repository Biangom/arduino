#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(2,3); // RX, TX
unsigned short arr[10] = {0};
 
void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  Serial.println("Goodnight moon!");
 
  // set the data rate for the SoftwareSerial port
  mySerial.begin(19200);
  arr[0] = 0x4C;
  arr[1] = 0x52;
  arr[2] = 0x57;  
  arr[3] = 0x20;
  arr[4] = 0x34;
  arr[5] = 0x43;
  
  arr[6] = 0x20;
  arr[7] = 0x0D;
  arr[8] = 0x0A;
}
 
void loop() // run over and over
{
  // Receive from bluetooth and send it to PC
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
    if (Serial.available()){
    mySerial.write(Serial.read());
  }    
}



//// Receive from PC and send it to bluetooth
//  for(int i = 0 ; i < 9 ; i++ ) {
//    mySerial.write(arr[i]);
//  }
//  delay(1000);

