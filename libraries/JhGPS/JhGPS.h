/*
	JhGPS.h - Library for using GPS 
	Created by JaeHo Jo, April 30, 2018
	Ver.GPS
*/
#ifndef JhGPS_h
#define JhGPS_h

#include "Arduino.h"

class JhGPS
{
	public :
		float f_lat, f_lon;			// 좌표값을 저장할 float 형 변수
	public :
		JhGPS(){
			f_lat = 0;
			f_lon = 0;
		}
		~JhGPS(){}
		
	public :
		void gpsParse();								// Gps Parse 
};

#endif






