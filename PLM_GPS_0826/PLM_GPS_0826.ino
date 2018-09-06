#include <TimerOne.h>
#include "JhGPS.h"
#define RESET_SEC 5
#define PERIOD 15
#define APERIOD 30

int second = 0;
int gps_second = 0;
int init_second = 0;
int accel_second = 0;
int data_second = 0;
int sw_pin = 7;
int buzzer = 6;
int isSw = 0;
bool isReset,isBuzzer = false;
String line;
bool istrans = false;

JhGPS gps; // Sk 라이브러리
// 리셋
void setup() {
    Serial.begin(115200);// Serial  : computer - arduino
    Serial1.begin(115200); // Serial1 : Arduino - LoRa
    Serial2.begin(9600);    // Serail2 : Arduino - GPS Module
  
    Timer1.initialize(500000000); // Timer 5s
    Timer1.attachInterrupt(interrupt);
   pinMode(sw_pin, INPUT_PULLUP);
    plm_start();
    delay(1000);
    plm_start();
    
}

void loop() {
    // LoRa - Arduino
    if (Serial1.available()) {
        // 1 Line씩 저장 
        line = Serial1.readStringUntil('\n');
    
       // Data 수신
        if(line.indexOf("RECV") != -1){
             // Emer 신호 ON
             if(line.indexOf("EMER_ON") != -1){
                //부저 On 함수 
               isBuzzer = true;
             } 

             // Emer 신호 OFF
             if(line.indexOf("EMER_OFF") != -1){
                //부저 On 함수 
                noTone(buzzer);
                isBuzzer = false;
             }
              
             //DevReset 시 Reset
              if(line.indexOf("DevReset")!= -1){
                // Timer로 5초 있다가  Reset 
                init_second = second;
                isReset = true;
             }
        }
        
        // Data 전송 실패 
        if(line.indexOf("(SUCCESS)") !=-1 || line.indexOf("(FAIL)") !=-1 ){
            istrans = false;
            if(line.indexOf("(FAIL)") !=-1 ){
                 plm_send();      
                data_second = second; 
                istrans = true;
            }
             if(line.indexOf("(SUCCESS)") !=-1 ){
                isSw=0;
            }
        }
    
        //LoRa 로그 출력
        Serial.println(line);
    }

    // Arduino - Computer 
    if(Serial.available()){
        // Serial 모니터에서 명령어 입력
        char user_cmd = Serial.read(); 
        Serial1.write(user_cmd);
    }
    
   // 25초마다 데이터 전송
    if( second - data_second>=PERIOD && !istrans ){
         plm_send();    
        data_second = second; 
        istrans = true;
    }

    // Reset
    if( second - init_second >= RESET_SEC && isReset){
        plm_rset();
        second = init_second = data_second = accel_second = 0;
        isReset = false;
     }

   // 스위치 On일경우
   if(digitalRead(sw_pin) == HIGH){
        isSw = -1;
   }

   // 부저 10초간 작동 
   if(isBuzzer){
        tone(buzzer,600);
        delay(100);
    }
    
    
   // 가속도 센서 측정 
   // 1초 동안 센서 작동 
   /*
   if( second - accel_second >= APERIOD && second - accel_second <= APERIOD+1){
        if(second - accel_second == 30) {
            accel_second = second;
        }
        // 가속도 수집
   }*/
}

// 60초 카운트를 위한 함수 
void interrupt(){
     second += 5;
}

// Rset
void plm_rset(){
    String cmd = "AT+RSET\n";
    for(int i = 0 ; i <cmd.length() ; i++){
        Serial1.write(cmd.charAt(i));
    }
    
}

// 데이터 전송 
// SW : 0 정상 , -1 긴급
void plm_send(){
    float lat, lon;
    String cmd = "AT+DATA=001c37:";
    
    gps.gpsParse(); //GPS Parse 호출

    //좌표값 저장
    lat = gps.f_lat;
    
    lon = gps.f_lon;
     
     cmd = cmd+lat+","+lon+"/"+isSw+"\n";
     for(int i = 0 ; i <cmd.length() ; i++){
             Serial1.write(cmd.charAt(i));
     } 
}

void plm_start(){
    String cmd = "AT+SHOW\n";
    for(int i = 0 ; i <cmd.length() ; i++){
        Serial1.write(cmd.charAt(i));
    }
}
/********************************/


