#include <TimerOne.h>
#include <Wire.h>
#include <TinyGPS.h>
#include <OneWire.h> 
#define RESET_SEC 5
#define PERIOD 15
#define APERIOD 10
#define GPSPERIOD 5
#define MPU_SDA_PIN 3
#define MPU_SCL_PIN 2


#include "kproject_mpu6050.h"
TinyGPS gps;
const int gpsEnablePin = 5;
int DS18S20_Pin = 2; //온도센서와 연결
int second = 0, gps_second = 0, init_second = 0, data_second = 0, buzzer_second  = 0, accel_second = 0;
int sw_pin = 7, buzzer = 6;
int isSw = 0;
bool isReset,isBuzzer = false;
String line;
bool istrans = false;
long cal = 0;
int g_count = 0;

float latitude = -1.00, longitude = -1.00;
OneWire ds(DS18S20_Pin); //2번 핀과 연결되 OneWire 객체 생성
// 활동량 측정
double diff; // val - prev_val;
unsigned long step_count = 0;
bool temp = true;

// 리셋
void setup() {
    Serial.begin(115200);// Serial  : computer - arduino
    Serial1.begin(115200); // Serial1 : Arduino - LoRa
    Serial2.begin(9600);    // Serail2 : Arduino - GPS Module
  
    Timer1.initialize(500000000); // Timer 5s
    Timer1.attachInterrupt(interrupt);
    pinMode(sw_pin, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);
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
               Serial.println("isBuzzer = True");
             } 
             
             // Warn 신호 OFF
             else if(line.indexOf("WARN_OFF") != -1){
                 isBuzzer = false;
                noTone(buzzer);
                isBuzzer = false;
                delay(10);
             }

             // Emer 신호 OFF
             if(line.indexOf("EMER_OFF") != -1){
                isSw = 0;
                noTone(buzzer);
                g_count = 0;
             }
             
             //DevReset 시 Reset
              if(line.indexOf("DevReset")!= -1){
                // Timer로 5초 있다가  Reset 
                init_second = second;
                isReset = true;
             }
        }
       /*
        // Data 전송 실패 
        if(line.indexOf("(SUCCESS)") !=-1 || line.indexOf("(FAIL)") !=-1 ){
           // istrans = false;
         /  if(line.indexOf("(FAIL)") !=-1 ){
           Serial.print("Lat/Long: "); 
          Serial.print(latitude,5); 
          Serial.print(", "); 
          Serial.println(longitude,5);
                 plm_send();      
                data_second = second; 
                istrans = true;
            }
        }*/
    
        //LoRa 로그 출력
        Serial.println(line);
    }

    // Arduino - Computer 
    if(Serial.available()){
        // Serial 모니터에서 명령어 입력
        char user_cmd = Serial.read(); 
        Serial1.write(user_cmd);
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
  
  
  
    // 25초마다 데이터 전송
    if( second - data_second>=PERIOD){
           Serial.print("Lat/Long: "); 
          Serial.print(latitude,5); 
          Serial.print(", "); 
          Serial.println(longitude,5);
          data_second = second; 
         plm_send();    
        
    }
   // 스위치 On일경우
   if(digitalRead(sw_pin) == HIGH){
        isSw = 1;
        
   }
   
   
      // 부저 10초간 작동 
   if(isBuzzer && isSw == 0){
        warn_syren();
   }
   if(isSw == 1){
        emer_syren();
   }


        while(Serial2.available())     // While there is data on the RX pin...
        {
            //latitude = -1.00, longitude = -1.00;
           int c = Serial2.read();    // load the data into a variable...
           //Serial.print(c);
           if(gps.encode(c))      // if there is a new valid sentence...
           {
                getgps(gps);         // then grab the data.
                if(latitude != -1.00 && longitude != -1.00){
                    break;
                }
            }   
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
     second += 10;
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
    
//    gps.gpsParse(); //GPS Parse 호출

    //좌표값 저장

     
     //cmd = cmd+"#"+lat+"#"+lon+"#"+isSw+"#"+step_count+"\n";
     
      lat = latitude;
      lon = longitude;
      float temperature = getTemp();  
      if(temperature >50) temperature = getTemp();  
      Serial.println(temperature);
     Serial1.print(cmd);
     Serial1.print(lat,8);
     Serial1.print("#");
     Serial1.print(lon,8);
     Serial1.print("#");
     Serial1.print(isSw);
     Serial1.print("#");
     Serial1.print(step_count);
      Serial1.print("#");
     Serial1.println(temperature);
     
     
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
    int count = 0 ;
    while(count <=2){
        for(int freq = 150; freq <=1800; freq = freq + 2) {
        tone(buzzer, freq, 10);
        }
    
         for(int freq = 1800; freq <=150; freq = freq - 2) {
        tone(buzzer, freq, 10);
        }
        delay(10);
        noTone(buzzer);
        
        count++;
    }
        delay(10);
        noTone(buzzer);  
}
void emer_syren(){
    int count = 0;
     while(count <=2){
       for(int i=200;i<=800;i++)
        {
            tone(buzzer,i); //Output the frequence in port 4
            delay(5); //Keep this frequence 5ms
        }
        delay(10); //Keep 4mins in the Highest frequence
        for(int i=800;i>=200;i--)
        {
            tone(buzzer,i);
            delay(10); //Keep this frequence 10ms
        }
        count++;
     }
        delay(10);
        noTone(buzzer);  
}

// The getgps function will get and print the values we want.
void getgps(TinyGPS &gps)
{
  // To get all of the data into varialbes that you can use in your code, 
  // all you need to do is define variables and query the object for the 
  // data. To see the complete list of functions see keywords.txt file in 
  // the TinyGPS and NewSoftSerial libs.
  
  // Define the variables that will be used

  // Then call this function
  gps.f_get_position(&latitude, &longitude);
      
  // You can now print variables latitude and longitude
  


  //Since month, day, hour, minute, second, and hundr
}



//온도 측정 후 반환하는 함수
float getTemp(){

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1);    //변환

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); 

 
 for (int i = 0; i < 9; i++) { 
  data[i] = ds.read();  //Scratchpad 읽음
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); 
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}
