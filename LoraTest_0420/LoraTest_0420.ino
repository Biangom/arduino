#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(2,3); // RX, TX
unsigned short lrw[4] = {0x4C, 0x52, 0x57, 0x20};
unsigned short cid[2]={0};
unsigned short option[5];
unsigned short crlf[3] = {0x20,0x0D,0x0A};
unsigned short AppKey[32] = {0x36, 0x66, 0x61, 0x38, 0x35, 0x37, 0x63, 0x31, 0x65,
                             0x33, 0x33, 0x33, 0x31, 0x39, 0x34, 0x36, 0x30, 0x30,
                             0x31, 0x31, 0x30, 0x33, 0x63, 0x32, 0x36, 0x36, 0x35, 
                             0x66, 0x31, 0x39, 0x65, 0x37};
 
void setup()  
{
  Serial.begin(19200);
  mySerial.begin(19200);
  Serial.println("Connection Start");
  reset();
 }
 
void loop() // run over and over
{
  String line;
  char ctl = '\0';
  if (mySerial.available()) {
    line = mySerial.readStringUntil('\n');
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
void transmission(){
  int i = 0 ; 
  char buf[5] = {'h','e','l','p'};
  cid[0] = 0x33;
  cid[1] = 0x31;
  option[0] = 0x20;
  option[1] = 0x63;
  option[2] = 0x6E;
  option[3] = 0x66;
  option[4] = 0x20;
  
  //LRW 입력
  for(i = 0 ; i < 4 ; i++) mySerial.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) mySerial.write(cid[i]);

  // space
  mySerial.write(crlf[0]);

  // 메세지 입력
   mySerial.write(buf);

  // Option 입력
  for(i = 0 ; i < 5 ; i++) mySerial.write(option[i]);

  // 1
  mySerial.write(cid[1]);
   
  // space
  mySerial.write(crlf[0]);
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) mySerial.write(crlf[i]);
  delay(100);
}

void command_nonOption(unsigned short CID1, unsigned short CID2){
  int i = 0 ; 
  cid[0] = CID1;
  cid[1] = CID2;

  //LRW 입력
  for(i = 0 ; i < 4 ; i++) mySerial.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) mySerial.write(cid[i]);


  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) mySerial.write(crlf[i]);
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
  for(i = 0 ; i < 4 ; i++) mySerial.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) mySerial.write(cid[i]);

  //Option 입력
  for(i = 0 ; i < 5 ; i++) {
    if(option[i] != 0)
      mySerial.write(option[i]);
    else continue;
  }

  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) mySerial.write(crlf[i]);
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
  for(i = 0 ; i < 4 ; i++) mySerial.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) mySerial.write(cid[i]);

  // space
  mySerial.write(crlf[0]);

  // EUI 입력
  for(i = 0 ; i < 16 ; i++){
    if(i < 15 ) mySerial.write(cid[0]);
    else mySerial.write(cid[1]);
  }
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) mySerial.write(crlf[i]);
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
  for(i = 0 ; i < 4 ; i++) mySerial.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) mySerial.write(cid[i]);

  // space
  mySerial.write(crlf[0]);

  // key 입력
  for(i = 0 ; i < 32 ; i++){
    mySerial.write(AppKey[i]);
  }

  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) mySerial.write(crlf[i]);
  delay(100);
}





