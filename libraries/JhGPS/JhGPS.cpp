#include "Arduino.h"
#include "JhGPS.h"

// GPS Parse
// GPS 모듈이 준 데이터 중 위도,경도 값만 Parsing
void JhGPS::gpsParse()
{
  String g_line=" ";  // Gps에서 받아들인 데이터를 저장할 String 변수
  String s_lat, s_lon; // 좌표값을 저장할 문자열 변수
 
  while(1){
	// 개행을 기준으로 한줄씩 저장   
    g_line = Serial2.readStringUntil('\n');
	Serial.println(g_line);
	//GPGLL로 시작하는 단어인가? ( GPS 모듈이 보내는 데이터 중 위도,경도 좌표값은 $GPGLL에 있음 )
    if(g_line.startsWith("$GPGLL")){
      
      //위도, 경도값을 s_lat, s_lon에 저장 
	  s_lat = g_line.substring(7,16);
      s_lon = g_line.substring(20,30);
      
	  // GPS에서 정상적으로 좌표값이 출력되지 않으면
      if(s_lat.charAt(0) != '3'){
        //f_lat, f_lon에 음수값 저장 
		f_lat = -1.00000;
        f_lon = -1.00000;
        return;
      }
       // 정상적인 좌표값이라면
	   // f_lat, f_lot에 좌표값 저장 후 자리수 조정
      else{
          f_lat = s_lat.toFloat();
          f_lon = s_lon.toFloat();
          f_lat /= 100.0;
          f_lon /= 100.0;
		  return;
       } 
     } 
  }
}									