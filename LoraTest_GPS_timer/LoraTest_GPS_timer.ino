#include<TimerOne.h>
#include <TinyGPS.h>
/*
#define RXPIN 6
#define TXPIN 5
#define GPSBAUD 9600


SoftwareSerial Serial1(2,3); // RX, TX
SoftwareSerial Serial2(RXPIN, TXPIN);
*/
TinyGPS gps;
unsigned short lrw[4] = {0x4C, 0x52, 0x57, 0x20};
unsigned short cid[2]={0};
unsigned short option[5];
unsigned short crlf[3] = {0x20,0x0D,0x0A};
unsigned short AppKey[32] = {0x36, 0x66, 0x61, 0x38, 0x35, 0x37, 0x63, 0x31, 0x65,
                             0x33, 0x33, 0x33, 0x31, 0x39, 0x34, 0x36, 0x30, 0x30,
                             0x31, 0x31, 0x30, 0x33, 0x63, 0x32, 0x36, 0x36, 0x35, 
                             0x66, 0x31, 0x39, 0x65, 0x37};
float latitude = 0.0, longitude = 0.0; // 위도, 경도

void getgps(TinyGPS &gps); 
void setup()  
{

  Serial.begin(19200);
   Serial1.begin(19200);
  Serial2.begin(9600);
 

  Timer1.initialize(1000000000); // 10초
  Timer1.attachInterrupt(checkgps);

  
 
  Serial.println("Connection Start");
  reset();
 }
 
void loop() // run over and over
{
  String line;
  char ctl = '\0';

  if (Serial1.available()) {
    line = Serial1.readStringUntil('\n');
    if(line.startsWith("DevReset")){
      delay(5000);
      reset();
    }
    Serial.println(line);
  }
  
  
  if(Serial.available()){
    ctl = Serial.read();
    switch(ctl){
      case 'r' : 
        reset();
        ctl = '\0';
        break;
      case 't':
        transmission();
        ctl = '\0';
        break;
      default :
        break;
    }
  }
}

// Message 전송
// LRW 31 Message cnt 1
// 10초마다 주기적으로 GPS 함수 실행해서 값을 받아오도록 설정해야함 
void transmission(){
  int i = 0 ; 
  char lat[8]={'n'};
  char lon[8]={'n'};
  cid[0] = 0x33;
  cid[1] = 0x31;
  option[0] = 0x20;
  option[1] = 0x63;
  option[2] = 0x6E;
  option[3] = 0x66;
  option[4] = 0x20;

  //float값으로 저장된 위도, 경도값을 char 배열로 옮김
//  if(checkgps()){
  checkgps();
    Serial.print('T');
    dtostrf(latitude,7,5,lat);
    dtostrf(longitude,7,5,lon);
  
  Serial.print(latitude);
  Serial.print(',');
  Serial.println(longitude);
//  }
//  else{
//    Serial.print('F');
//    
//    Serial.print(latitude);
//    Serial.print(',');
//    Serial.println(longitude);
//  }
  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  // space
  Serial1.write(crlf[0]);

  // 메세지 입력
  //{위도,경도}로 전송
   Serial1.write(lat);
   Serial1.write(',');
   Serial1.write(lon);

  // Option 입력
  for(i = 0 ; i < 5 ; i++) Serial1.write(option[i]);

  // 1
  Serial1.write(cid[1]);
   
  // space
  Serial1.write(crlf[0]);
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
  delay(1000);
}

void command_nonOption(unsigned short CID1, unsigned short CID2){
  int i = 0 ; 
  cid[0] = CID1;
  cid[1] = CID2;

  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);


  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
}


void command_Option(unsigned short CID1, unsigned short CID2, unsigned short option1 = 0, unsigned short option2 = 0, unsigned short option3 = 0, unsigned short option4 = 0){
  int i = 0 ; 
  cid[0] = CID1;
  cid[1] = CID2;
  option[0] = 0x20;
  option[1] = option1;
  option[2] = option2;
  option[3] = option3;
  option[4] = option4;
  
  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  //Option 입력
  for(i = 0 ; i < 5 ; i++) {
    if(option[i] != 0)
      Serial1.write(option[i]);
    else continue;
  }

  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
}

// LoRa Reset
// LRW 70 CR LF
void reset(){
  command_nonOption(0x37,0x30);
  delay(100);
}

// AppEUI Setting
// LRW 33 0000000000000004(hexa) CR LF
void set_appeui(){
  command_Option(0x37,0x30,0x30,0x34);
  int i = 0 ; 
  cid[0] = 0x33;
  cid[1] = 0x33;

  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  // space
  Serial1.write(crlf[0]);

  // EUI 입력
  for(i = 0 ; i < 16 ; i++){
    if(i < 15 ) Serial1.write(cid[0]);
    else Serial1.write(cid[1]);
  }
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
  delay(100);
}

// Class Type Setting
// LRW 4B 2 ( C TYpe )
void set_classtype(){
  command_Option(0x37,0x30,0x32);
  delay(100);
}

// Application Key Setting
// LRW 51 AppKey CR LF
void set_appkey(){
  int i = 0 ; 
  cid[0] = 0x35;
  cid[1] = 0x31;

  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  // space
  Serial1.write(crlf[0]);

  // key 입력
  for(i = 0 ; i < 32 ; i++){
    Serial1.write(AppKey[i]);
  }

  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
  delay(100);
}

// GPS값 정상적으로 받아왔는지 확인
// 정상이면 true, 아니면 false
void checkgps(){
  while(Serial2.available()){
    int c = Serial2.read();    // load the data into a variable...
    if(gps.encode(c))      // if there is a new valid sentence...
    {
      Serial.print('e');
      getgps(gps);         // then grab the data.
      //return true;
    } 
    //else return false;  
  }
  //return true;
  
}
// GPS값 받아오기
void getgps(TinyGPS &gps)
{
  // To get all of the data into varialbes that you can use in your code, 
  // all you need to do is define variables and query the object for the 
  // data. To see the complete list of functions see keywords.txt file in 
  // the TinyGPS and NewSoftSerial libs.
  
  // Define the variables that will be used

  //int switch_state = digitalRead(sw_out);
  // Then call this function
  gps.f_get_position(&latitude, &longitude);
  // You can now print variables latitude and longitude
  Serial.print("Lat/Long: "); 
  Serial.print(latitude,5); 
  Serial.print(", "); 
  Serial.println(longitude,5);
  /*
   Serial.print("Switch State : ");
  Serial.println(switch_state);
  */
  
  // Same goes for date and time
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
  // Print data and time
  Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/"); 
  Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
  Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
  Serial.print("."); Serial.println(hundredths, DEC);
  //Since month, day, hour, minute, second, and hundr
  
  // Here you can print the altitude and course values directly since 
  // there is only one value for the function
  Serial.print("Altitude (meters): "); Serial.println(gps.f_altitude());  
  // Same goes for course
  Serial.print("Course (degrees): "); Serial.println(gps.f_course()); 
  // And same goes for speed
  Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
  Serial.println();
  
  // Here you can print statistics on the sentences.
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
  Serial.print("Failed Checksums: ");Serial.print(failed_checksum);
  Serial.println(); Serial.println();
  delay(5000);
}



