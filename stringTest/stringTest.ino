#include <SoftwareSerial.h>

byte _buffer[1024] = "123";
int x = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.write(_buffer,4);

}
