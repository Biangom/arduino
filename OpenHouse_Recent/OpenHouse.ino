 int sw_out = 12; 
 int led = 13;
void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   pinMode(sw_out,INPUT);
    pinMode(led,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float latitude, longitude;
  int swstate = 0;
  bool emergency = false;
  
  //서울
  latitude = 37.5650168;
  longitude = 126.849123;

  //스위치가 High일때
  if( digitalRead(sw_out) == 1 ) swstate = 1;
  //스위치가 Low일때
  else swstate = 0;

  //Emergency Signal이 True 이면 내부 LED 깜박임
  if(emergency) {
    for(int i = 0; i < 3 ; i++){
      digitalWrite(led,HIGH);
      delay(400);
      digitalWrite(led,LOW);
      delay(400);
    }
   
  }
  else digitalWrite(led,LOW);

    Serial.print("LRW 31 ");
    Serial.print(latitude,5);
    Serial.print(",");
    Serial.print(longitude,5);
    Serial.print(",");
    Serial.print(swstate);
    Serial.println(" cnf 1");
    
  

    delay(2000);

  
  
  
}


