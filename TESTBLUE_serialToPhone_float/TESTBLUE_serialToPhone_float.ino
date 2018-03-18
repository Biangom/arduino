#include <SoftwareSerial.h>
  
SoftwareSerial BTSerial(2, 3); // SoftwareSerial(RX, TX)
byte buffer[1024]; // 데이터를 수신 받을 버퍼
int bufferPosition; // 버퍼에 데이타를 저장할 때 기록할 위치

float one = 36.76633;
float two = 127.28124;

byte _buffer[100];
int _bufferPosition;

void setup(){
  BTSerial.begin(9600); 
  Serial.begin(9600); 
  bufferPosition = 0; // 버퍼 위치 초기화
  
  dtostrf(one, 8, 5, _buffer);
  
  _bufferPosition = 8;
  _buffer[8] = '\0';
}
 
void loop(){
  delay(1000);
  BTSerial.write("1");
  Serial.write("1");

  /*
  if (Serial.available()){ // 블루투스로 데이터 수신
    byte data = Serial.read(); // 수신 받은 데이터 저장
    Serial.write(data); // 수신된 데이터 시리얼 모니터로 출력
    buffer[bufferPosition++] = data; // 수신 받은 데이터를 버퍼에 저장
  
    if(data == '\n'){ // 문자열 종료 표시
      buffer[bufferPosition] = '\0';
 
      // 스마트폰으로 문자열 전송
      BTSerial.write(buffer, bufferPosition);
      bufferPosition = 0;
    }  
  }
  */
  //36.76633,127.28124

  if (BTSerial.available())
  {
    Serial.write(BTSerial.read());
  }
}

