
#include <SoftwareSerial.h>


SoftwareSerial LoRa(6, 7); // RX, TX  

char line;
char cl;
void setup() {
   LoRa.begin(115200);
  Serial.begin(19200);
}

void loop() {
    if(LoRa.available()){
        line = LoRa.read();
        Serial.write(line);
    }
    if(Serial.available()){
        cl = Serial.read();
        LoRa.write(cl);
    }
}

