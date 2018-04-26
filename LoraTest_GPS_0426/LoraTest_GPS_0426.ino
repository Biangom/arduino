#include<TimerOne.h>
#include <TinyGPS.h>

TinyGPS gps;
unsigned short lrw[4] = {0x4C, 0x52, 0x57, 0x20};
unsigned short cid[2]={0};
unsigned short option[5];
unsigned short crlf[3] = {0x20,0x0D,0x0A};
unsigned short AppKey[32] = {0x36, 0x66, 0x61, 0x38, 0x35, 0x37, 0x63, 0x31, 0x65,
                             0x33, 0x33, 0x33, 0x31, 0x39, 0x34, 0x36, 0x30, 0x30,
                             0x31, 0x31, 0x30, 0x33, 0x63, 0x32, 0x36, 0x36, 0x35, 
                             0x66, 0x31, 0x39, 0x65, 0x37};

bool check = false;
float f_lat = 0.0, f_lon = 0.0;
void getgps(TinyGPS &gps); 
void setup()  
{
  Serial.begin(19200);
  Serial1.begin(19200);
  Serial2.begin(9600);
 
  Timer1.initialize(500000000); // 10초
  Timer1.attachInterrupt(checkplus);

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
   
    switch(Serial.read()){
      case 'r' : 
        reset();
        ctl = '\0';
        break;
      case 't':
        break;
      default :
        break;
    }
  }
  if(check){
    transmission();
    check = false;
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

  checkgps();
  Serial.print(f_lat,5);
  Serial.print(',');
  Serial.println(f_lon,5);
 
  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  // space
  Serial1.write(crlf[0]);

  // 메세지 입력
  //{위도,경도}로 전송
   Serial1.print(f_lat,5);
   Serial1.print(',');
   Serial1.println(f_lon,5);

  // Option 입력
  for(i = 0 ; i < 5 ; i++) Serial1.write(option[i]);

  // 1
  Serial1.write(cid[1]);
   
  // space
  Serial1.write(crlf[0]);
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
 // delay(1000);
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
  String line2=" "; 
  String s_lat, s_lon;
  while(1){
    line2 = Serial2.readStringUntil('\n');
    if(line2.startsWith("$GPGLL")){
      Serial.println(line2);
      s_lat = line2.substring(7,17);
      s_lon = line2.substring(20,31);
      if(s_lat.charAt(0) != '3'){
        f_lat = -1;
        f_lon = -1;
        return;
      }
       else{
           f_lat = s_lat.toFloat();
           f_lon = s_lon.toFloat();
           f_lat /= 100;
           f_lon /= 100;
           return;
       } 
     } 
  }
}
void checkplus(){
  check = true;
}





