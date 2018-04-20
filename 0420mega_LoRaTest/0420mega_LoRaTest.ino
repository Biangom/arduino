
unsigned short arr[10] = {0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);

  arr[0] = 0x4C;
  arr[1] = 0x52;
  arr[2] = 0x57;  
  arr[3] = 0x20;
  arr[4] = 0x34;
  arr[5] = 0x43;
  
  arr[6] = 0x20;
  arr[7] = 0x0D;
  arr[8] = 0x0A;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    if( Serial.read() == 'e' ) {
      classSetting();
    }
  }
  
  if(Serial1.available()){
    Serial.write(Serial1.read());
  }
}

void classSetting() { 
  for(int i = 0 ; i < 9 ; i++ ) {
     Serial1.write(arr[i]);
  }
}
