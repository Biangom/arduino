
#include <SoftwareSerial.h>



char line;
char cl;
void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop() {
    if(Serial1.available()){
        line = Serial1.read();    
        Serial.write(line);
    }
    if(Serial.available()){
        cl = Serial.read();
        Serial1.write(cl);
    }
}

