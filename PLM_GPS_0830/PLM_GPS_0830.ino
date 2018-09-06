#include <TimerOne.h>
#include <Wire.h>
#include "JhGPS.h"

#define RESET_SEC 5
#define PERIOD 10
#define APERIOD 10
#define MPU_SDA_PIN 3
#define MPU_SCL_PIN 2


#include "kproject_mpu6050.h"

int second = 0, gps_second = 0, init_second = 0, data_second = 0, buzzer_second  = 0, accel_second = 0;
int sw_pin = 7, buzzer = 6;
int isSw = 0;
bool isReset,isBuzzer = false;
String line;
bool istrans = false;
long cal = 0;

// 활동량 측정
double diff; // val - prev_val;
unsigned long step_count = 0;
bool temp = true;

JhGPS gps; // GPS 라이브러리

// 리셋
void setup() {
    Serial.begin(115200);// Serial  : computer - arduino
    Serial1.begin(115200); // Serial1 : Arduino - LoRa
    Serial2.begin(9600);    // Serail2 : Arduino - GPS Module
  
    Timer1.initialize(500000000); // Timer 5s
    Timer1.attachInterrupt(interrupt);
    pinMode(sw_pin, INPUT_PULLUP);
    plm_start();
    delay(500);
    plm_start();

}

void loop() {
    // LoRa - Arduino
    if (Serial1.available()) {
        // 1 Line씩 저장 
        line = Serial1.readStringUntil('\n');
    
       // Data 수신
        if(line.indexOf("RECV") != -1){
              // Warn 신호 ON
             if(line.indexOf("WARN_ON") != -1){
               isBuzzer = true;
             } 
             
             // Warn 신호 OFF
             if(line.indexOf("WARN_OFF") != -1){
                noTone(buzzer);
                isBuzzer = false;
             }

             // Emer 신호 OFF
             if(line.indexOf("EMER_OFF") != -1){
                isSw = 0;
                noTone(buzzer);
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
/*
    // 전송 후 20초 지나면 재전송을 판단 ( 시간초 초기화 )
    // 전송 후 재전송 거쳐서 Fail뜨는게 35초 걸림 ( 그 사이에 데이터 전송을 막고자 )
    if( second - timer_second >= 20 && istrans ){
        timer_second = 0;
    }*/

    // Reset
    if( second - init_second >= RESET_SEC && isReset){
        plm_rset();
        second = init_second = data_second = accel_second = 0;
        isReset = false;
     }

   // 스위치 On일경우
   if(digitalRead(sw_pin) == HIGH){
        isSw = 1;
        
   }
   
    if(isSw == 1 ){
        cal = millis()/1000;
        if(cal % 2 == 0) tone(buzzer,783);
        else tone(buzzer,880);
    }

    
    // 부저 10초간 작동 
   if(isBuzzer && isSw == 0){
        warn_syren();
   }
   
  
  
   
    /*
   // 가속도 센서 측정 
   // 15초마다 측정
   if( second - accel_second >= APERIOD){
        accel_second = second;
        accel();
   }
    */
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
    String cmd = "AT+DATA=001c37:#";
    
    gps.gpsParse(); //GPS Parse 호출

    //좌표값 저장
    lat = gps.f_lat;
    
    lon = gps.f_lon;
     
     //cmd = cmd+"#"+lat+"#"+lon+"#"+isSw+"#"+step_count+"\n";
     
        Serial.print(lat,5);
        Serial.print(",");
        Serial.println(lon,5);
        
     Serial1.print(cmd);
     Serial1.print(lat,5);
     Serial1.print("#");
     Serial1.print(lon,5);
     Serial1.print("#");
     Serial1.print(isSw);
     Serial1.print("#");
     Serial1.println(step_count);
     
     /*
     for(int i = 0 ; i <cmd.length() ; i++){
             Serial1.write(cmd.charAt(i));
     } /*
     Serial1.write('#');
     Serial1.write(lat,5);
    Serial1.write('#');
     Serial1.write(lon,5);*/
     
}

void plm_start(){
    String cmd = "AT+SHOW\n";
    for(int i = 0 ; i <cmd.length() ; i++){
        Serial1.write(cmd.charAt(i));
    }
}

void accel(){
    double ax, ay, az, gx, gy, gz;
    int sensing_count = 0, valid_count = 0;
    double prev_val = 0, val = 0;
    while(1){
        // 가속도값 추출
        ax = GetData(ACCEL_XOUT_H);
        ay = GetData(ACCEL_YOUT_H);
        az = GetData(ACCEL_ZOUT_H);
        ax = ax / 1000;
        ay = ay / 1000;
        az = az / 1000;

        // 제곱합 -> 루트
        val = sqrt(ax * ax + ay * ay + az * az);
        val = (val - 17); // 16을 뺀 이유는 중력값을 빼기 위함
        Serial.println(val);
        // 음수값은 양수로 변환
        if(val < 0.0) val = val * -1;
  
        // 이전값과의 diff( 차이 )비교
        diff = val - prev_val;

        // diff < 0 일때 = 양수로 변환
        if(diff < 0.0) diff *= -1;

        // diff의 크기가 2 이상일때 ( 걸었다고 판단 )
        if( diff >= 2.0) {
            valid_count++; 
        }
        sensing_count++; // 10번까지 측정 
    
        // 10번 센싱을 한다.
        // 10번 센싱을 다 채웠으면,
        // 10번 중에 유효한 카운트 비율이 0.4 이상이면 step을 증가
        if(sensing_count == 10) {
            if( (double)valid_count / (double)step_count >= 0.4) step_count++; // 걸음수 증가
            
            // count변수들을 초기화한다.
            sensing_count = 0;
            valid_count = 0;
            //Serial.println(step_count);
            //delay(2500);
            break;
        }
        delay(25);
   }
}

void warn_syren(){
    float sinVal; //정현파 값 저장
    int toneVal; //경보음 발생을 위한 값 저장
    for(int i=0;i<180;i++)
   {
        sinVal=sin(i*PI/180); //각도를 라디언으로 바꿔 정현파 값 계산
        toneVal=(int)(100+1000*sinVal); //경보음 주파수(Hz)로 변환
        tone(buzzer,toneVal);//피에조 스피커에서 주파수 발생
        delay(10); //경보음 주파수 변화 속도 조정
    }
}




