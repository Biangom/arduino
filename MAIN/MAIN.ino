#include <SoftwareSerial.h>
  
SoftwareSerial BTSerial(2, 3); // SoftwareSerial(RX, TX)
byte buffer[512]; // 데이터를 수신 받을 버퍼
int bufferPosition; // 버퍼에 데이타를 저장할 때 기록할 위치

byte _buffer[512]; // 임시 버퍼
int _pos; // 임시 버퍼 포지션

double one = 36.76633;
double twp = 127.28124;

void setup(){
  BTSerial.begin(9600); 
  Serial.begin(9600); 
  bufferPosition = 0; // 버퍼 위치 초기화
  _pos = 0;

  //dtostrf(one, 

  _buffer[0] = '1';
  _buffer[1] = '2';
  _buffer[2] = '\n';

  
}
 
void loop(){
 
  if (Serial.available()){ // 블루투스로 데이터 수신
    byte data = Serial.read(); // 수신 받은 데이터 저장
    Serial.write(data); // 수신된 데이터 시리얼 모니터로 출력
    
    //_buffer[_pos] = data; // 수신 받은 데이터를 버퍼에 저장
    buffer[bufferPosition] = _buffer[_pos];
    ++bufferPosition, ++_pos;
  
    if(buffer[bufferPosition - 1] == '\n') { // 문자열 종료 표시
      buffer[bufferPosition] = '\0';
 
      // 스마트폰으로 문자열 전송
      
      //BTSerial.write(buffer, bufferPosition);
      BTSerial.print(one);
      BTSerial.print("\n");
      bufferPosition = 0;
      _pos = 0;
    }  
    
  }
  
  //36.76633,127.28124

  if (BTSerial.available())
  {
    Serial.write(BTSerial.read());
  }
  
}

