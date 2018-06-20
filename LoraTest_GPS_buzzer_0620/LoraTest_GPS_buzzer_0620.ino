#include "SkLoRa.h"
#include <TimerOne.h>

SkLoRa sk; // Sk 라이브러리
bool b_switch = false; 
bool b_up = false; // UpConfirmed인 경우
//int sw_count = 0; 
int cmd_cnt  = 0; // cmd 배열의 index
int send_cnt = 0; 
int second = 0;
int gps_second = 0;
int send_second = 0;
bool resend_check = false; // 다시 전송했는지 check
char cmd[100];

void setup()  
{
  Serial.begin(19200);  // Computer - Arduino
  Serial1.begin(19200); // Arduino - LoRa Module
  Serial2.begin(9600);  // Arduino - GPS Module
 
  Timer1.initialize(1000000000); // 10초 
  Timer1.attachInterrupt(interr);
  
  Serial.println("Connection Start");
  //delay(1000);
  init_cmd();
  sk.reset();
 }
 
void loop() // run over and over
{
  String line;
  char ctl = '\0';

  //LoRa에서 디버깅 메세지 전송했을 때 
  if (Serial1.available()) {
    //개행을 기준으로 Line으로 전송
    line = Serial1.readStringUntil('\n');
    //DevReset 시 Reset
    if(line.startsWith("DevReset")){
      delay(10000); //10s delay
      sk.reset();
    }
    //Join 완료 시
    if(line.startsWith("Join is completed")){
      sk.b_join = true;
    }
    
    //Confirmed_UP 메세지 수신시 ( 메세지 처음 전송 시 )
    if(line.indexOf("CONFIRMED_UP") != -1){
      b_up = true;
    }

    //Transmission 이후 BUSY라고 뜰때 ( 전송 안댐 )
    if(line.startsWith("BUSY")){
      resend_check = false;
      gps_second = second;
    }
    
    //Confirmed_up 이후 데이터가 재전송 중일 때
    if(b_up){
      // UNCONFIRMED_DOWN 메세지 수신시 ( Confirmed_up에 대한 ACK )
      if(line.indexOf("UNCONFIRMED_DOWN") != -1){
        gps_second = second; //60초 타이머
        resend_check = false;
        b_up = false; // 재전송 끝
        send_cnt = 0;
      }
      
      //SEND 한 경우 ( 재전송일 때 - 최초 전송 포함 )
      if(line.indexOf("SEND") != -1 && b_up) send_cnt++;

      //재전송을 8번 했을 때 
      if(send_cnt == 8){
        gps_second = second; //60초 타이머
        resend_check = false;
        b_up = false; //재전송 끝
        send_cnt = 0;
      }
    }
    
   /*
    //Emergency 신호 수신 시 ( 16진수값 )
    if(line.startsWith("TotalTxTime")){
      sk.buzzer(9,10);
    }*/
    //LoRa 로그 출력
    Serial.println(line);
    if(send_cnt != 0){
      Serial.print("Send_cnt : " );
      Serial.println(send_cnt);
    }
    Serial.print("b_up : " );
      Serial.println(b_up);
    
    }
  
 
  if(Serial.available()){
    char user_cmd = Serial.read(); // Serial 모니터에서 명령어 입력 

    // - : LinkCheckRequest
    if(user_cmd == '-'){
      sk.setLinkCheckRequest();      
    }
    
    // + : TimeSyncRequest
    else if(user_cmd == '+'){
      sk.setTimeSyncRequest();
    }
    
    // * : 65Byte Message 전송
    else if(user_cmd == '*'){
      sk.transmission_test('a');
    }

    // / : 66Byte Message 전송
    else if(user_cmd == '/'){
      sk.transmission_test('b');
    }
    
    // 엔터 :  배열을 통째로 전송
    else if(user_cmd == '\n'){
      short crlf[2] = {0x0D,0x0A}; //CR, LF
      
      // cmd배열 첫번째가 Null이 아닐때 ( CLI 명령어를 입력했을 때 )
      if(cmd[0] != '\0' ) {
        // 65바이트인 경우
        if( cmd[78] == '\0'){
          //명령어 입력
          for(int i = 0 ; i < 100; i++){
            Serial1.write(cmd[i]); //명령어 입력
            if(cmd[i] == '\0') {
              for(int j = 0 ; j<2; j++) Serial1.write(crlf[j]);
              init_cmd(); // 배열 초기화
              break;
          }
        }
       }  
       // 65바이트 초과
        else{
         Serial.println("\nSize is Over");
         init_cmd(); // 배열 초기화
       }
      }
      
    }
    //명령어를 배열에 저장
    else {
       cmd[cmd_cnt] = user_cmd;
       cmd_cnt++;
    }
    
  }

   // 60초 && Join되어있을 경우만
   if( second - gps_second >= 60 && resend_check == false && sk.b_join){
     //sk.transmission_gps();
      resend_check = true;    
  }
}



// 60초 카운트를 위한 함수 
void interr(){
  second += 10;
}

// cmd 배열 초기화 
void init_cmd(){
   for(int i = 0 ; i < 50; i++) cmd[i] = '\0';
   cmd_cnt = 0;//cmd 배열의 Index
}





  /*
  부저 함수화 해야함
  //if(sk.b_buzzer && sk.b_join){
    if((sk.b_buzzer){
      //tone(PIN, 1000); // 5옥타브 도
      sk.b_buzzer = false;
  }
  //스위치
   if(sk.switchon(2) == LOW && count == 0){  
      sk.transmission_switch();
      count = 1;   
   }
}
*/
