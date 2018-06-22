#include "Arduino.h"
#include "SkLoRa.h"


// Class Type Setting
// LRW 4B 2 ( C TYpe )
void SkLoRa::setclass()
{
	command(0x37,0x30,0x32);
	delay(100);
}

// Application Key Setting
// LRW 51 AppKey CR LF
void SkLoRa::setappkey()
{
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

// AppEUI Setting
// LRW 33 0000000000000004(hexa) CR LF
void SkLoRa::setappeui()
{
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

// Link Check Request 
// LRW  38
void SkLoRa::setLinkCheckRequest()
{
	command(0x33,0x38);
	delay(100);
}

// Debug Message On/Off
// LRW 50 0 / 1 ( 0 : Off, 1 : On )
void SkLoRa::setDebugmode(bool mode)
{
	unsigned short option;
	if(mode) option = 0x31;
	else option = 0x30;
	command(0x35,0x30,option);
	delay(100);
}

// Time Sync Request
// LRW 39
void SkLoRa::setTimeSyncRequest()
{
	command(0x33,0x39);
}


// LoRa Reset
// LRW 70 CR LF
void SkLoRa::reset()
{
	command(0x37,0x30);
	b_join = false;
	delay(100);
}

// Serial connection Check
// LRW 49
void SkLoRa::check_connection()
{
	command(0x34,0x39);
	delay(100);
}


// GPS Parse
// GPS 모듈이 준 데이터 중 위도,경도 값만 Parsing
void SkLoRa::gpsParse()
{
  String g_line=" ";  // Gps에서 받아들인 데이터를 저장할 String 변수
  String s_lat, s_lon; // 좌표값을 저장할 문자열 변수
  //initlatlon();
  while(1){
	// 개행을 기준으로 한줄씩 저장   
    g_line = Serial2.readStringUntil('\n');
	
	//GPGLL로 시작하는 단어인가? ( GPS 모듈이 보내는 데이터 중 위도,경도 좌표값은 $GPGLL에 있음 )
    if(g_line.startsWith("$GPGLL")){
      //Serial.println(g_line);
      //위도, 경도값을 s_lat, s_lon에 저장 
	  s_lat = g_line.substring(7,17);
      s_lon = g_line.substring(20,31);
      
	  // GPS에서 정상적으로 좌표값이 출력되지 않으면
      if(s_lat.charAt(0) != '3'){
        //f_lat, f_lon에 음수값 저장 
		f_lat = -1;
        f_lon = -1;
        return;
      }
       // 정상적인 좌표값이라면
	   // f_lat, f_lot에 좌표값 저장 후 자리수 조정
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


// Check Firmware Version
// LRW 4F
void SkLoRa::getVersion()
{
	command(0x34,0x46);
	delay(100);
}

// Message 전송
// LRW 31 Message cnt 1
// 10초마다 주기적으로 GPS 함수 실행해서 값을 받아오도록 설정해야함 
void SkLoRa::transmission_gps()
{
  int i = 0 ; 
  unsigned short option[5];
  
  // lat : 위도, lon : 경도
  char lat[8]={'n'};
  char lon[8]={'n'};
  
  //CID
  cid[0] = 0x33;
  cid[1] = 0x31;
  
  //Option : cnf
  option[0] = 0x20;
  option[1] = 0x63;
  option[2] = 0x6E;
  option[3] = 0x66;
  option[4] = 0x20;
	
  gpsParse(); // f_lat, f_lon에 좌표값 저장

  
  // Gps값 정상 출력 Test Code
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
  // 위도,경도 -> 전송 형식, 추후 스위치 같은 경우는 T/F위도,경도
  Serial1.print(f_lat,5);
  Serial1.print(',');
  Serial1.print(f_lon,5);
  
  // Option 입력
  for(i = 0 ; i < 5 ; i++) Serial1.write(option[i]);

  // 1
  Serial1.write(cid[1]);
   
  // space
  Serial1.write(crlf[0]);
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
}

// Message 전송
// LRW 31 Message cnt 1
// Switch Message 전송
void SkLoRa::transmission_switch()
{
  int i = 0 ; 
  unsigned short option[5];
  char dummy[60];
  
  for( i = 0 ; i< 60 ; i++){
	  dummy[i] = '1';
  }
  //CID
  cid[0] = 0x33;
  cid[1] = 0x31;
  
  //Option : cnf
  option[0] = 0x20;
  option[1] = 0x63;
  option[2] = 0x6E;
  option[3] = 0x66;
  option[4] = 0x20;
  
  Serial.print("SwitchOn");
  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  // space
  Serial1.write(crlf[0]);

  // 메세지 입력
  // 위도,경도 -> 전송 형식, 추후 스위치 같은 경우는 T/F위도,경도
   Serial1.print(dummy);
  
  // Option 입력
  for(i = 0 ; i < 5 ; i++) Serial1.write(option[i]);

  // 1
  Serial1.write(cid[1]);
   
  // space
  Serial1.write(crlf[0]);
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
}

// Message 전송
// LRW 31 Message cnt 1
// Test 전송
void SkLoRa::transmission_test(char dummy)
{
  int i = 0 ; 
  bool tran_check = false;
  unsigned short option[5];
  char arr[70];
  for(i = 0 ; i< 70 ; i++) arr[i] = 0;
  if( dummy == 'a'){
	  for( i = 0 ; i < 65 ; i++) arr[i] = dummy;
  }
  else{
	  for( i = 0 ; i < 70 ; i++) arr[i] = dummy;
  }
  
	if(arr[65] != 0) {
		Serial.println("Size is Over!");
		return;
	}
  
  //CID
  cid[0] = 0x33;
  cid[1] = 0x31;
  
  //Option : cnf
  option[0] = 0x20;
  option[1] = 0x63;
  option[2] = 0x6E;
  option[3] = 0x66;
  option[4] = 0x20;
	

 
  //LRW 입력
  for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

  //CID 입력
  for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);

  // space
  Serial1.write(crlf[0]);

 
 for(i = 0 ; i < 65 ; i++) Serial1.write(arr[i]); 

 
  // Option 입력
  for(i = 0 ; i < 5 ; i++) Serial1.write(option[i]);

  // 1
  Serial1.write(cid[1]);
  
  //CRLF 입력
  for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
}



// buzzer 작동
// 몇 초동안 작동시킬지 구현 안함
void SkLoRa::buzzer(int b1, int b2)
{
	//tone(b1, 500,500); // 5옥타브 도
	//noTone(b1);
	tone(10, 500,3000); // 5옥타브 도
	noTone(10);
}

int SkLoRa::switchon(int b1){
	int sensorValue = digitalRead(2);

	return sensorValue;
}
// Command 입력 기본 형식 
// CID = CID1, CID2, Option은 최대 4개까지 ( default = 0 )
void SkLoRa::command(unsigned short CID1, unsigned short CID2, unsigned short option1 = 0, unsigned short option2 = 0, unsigned short option3 = 0, unsigned short option4 = 0, unsigned short option5 = 0)
{
	int i = 0;
	unsigned short cid[2];
	unsigned short option[6];
	
	cid[0] = CID1;
	cid[1] = CID2;
	option[0] = 0x20;
	option[1] = option1;
	option[2] = option2;
	option[3] = option3;
	option[4] = option4;
	option[5] = option5;
	
	//LRW 입력
	for(i = 0 ; i < 4 ; i++) Serial1.write(lrw[i]);

	//CID 입력
	for(i = 0 ; i < 2 ; i++) Serial1.write(cid[i]);
	
	//option 입력
	if(option[1] != 0)
	{
		for(i = 0 ; i < 6 ; i++)
		{
			if(option[i] != 0)
			{
				Serial.write(option[i]);
			}
		}
	}
	
	//CRLF 입력
	for(i = 0 ; i < 3 ; i++) Serial1.write(crlf[i]);
}

