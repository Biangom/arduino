/*
	SkLoRa.h - Library for using SkLoRa , Include GPS Transmission
	Created by JaeHo Jo, April 30, 2018
	Ver.GPS
*/

#ifndef SkLoRa_h
#define SkLoRa_h

#include "Arduino.h"

class SkLoRa
{
	public :
		unsigned short lrw[4] 		= {0x4C, 0x52, 0x57, 0x20};; 		// LRW
		unsigned short cid[2] 		= {0}; 								// CID ( Two Number )
		unsigned short crlf[3] 		= {0x20,0x0D,0x0A};					// CR LF
		unsigned short AppKey[32]	= {0x36, 0x66, 0x61, 0x38, 0x35, 0x37, 0x63, 0x31, 
									   0x65, 0x33, 0x33, 0x33, 0x31, 0x39, 0x34, 0x36, 
									   0x30, 0x30, 0x31, 0x31, 0x30, 0x33, 0x63, 0x32, 
									   0x36, 0x36, 0x35, 0x66, 0x31, 0x39, 0x65, 0x37};  // AppKey
		float f_lat, f_lon;			// 좌표값을 저장할 float 형 변수
		int sw_count;
		bool b_join, b_buzzer, b_debug;      // join 완료시 True
	
	public :
		SkLoRa(){
			b_join = false;
			b_buzzer = false;
			b_debug = false;
			sw_count = 0;
			f_lat = 0;
			f_lon = 0;
		}
		
		~SkLoRa(){}
		
	public :
		void setclass();								// class Setting
		void setappkey();								// Application Key Setting
		void setappeui();								// App EUI Setting
		void setLinkCheckRequest(); 					// Link Check Request
		void setDebugmode(bool mode);					// Set Debug Message On/Off
		void setTimeSyncRequest();  					// Time Sync Request
		void check_connection();						// Check Connection
		void reset();									// Reset
		void transmission_gps();						// GPS Message Transmission ( GPS Latitude, longitude )
		void transmission_switch();						// Switch Message Transmission
		void transmission_test(char a);						// Test Transmission 
		void gpsParse();								// Gps Parse 
		void getVersion();								// Get Firmware Version
		void buzzer(int b1, int b2);					// Activate Buzzer
		int switchon(int b1);
		void command(unsigned short CID1, unsigned short CID2, unsigned short option1 = 0, 
					 unsigned short option2 = 0, unsigned short option3 = 0, unsigned short option4 = 0, unsigned short option5 = 0); // Standard Command Form
};

#endif






