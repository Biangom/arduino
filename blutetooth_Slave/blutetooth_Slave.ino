#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); //블루투스의 Tx, Rx핀을 2번 3번핀으로 설정

const int xPin = 0;
const int yPin = 1;
const int zPin = 2;

int minVal = 265;
int maxVal = 402;
//to hold the caculated values
double x;
double y;
double z;
void setup(){
    Serial.begin(9600);
    while (!Serial) {
     ; //시리얼통신이 연결되지 않았다면 코드 실행을 멈추고 무한 반복
    }
    mySerial.begin(9600);
}
void loop(){
    //read the analog values from the accelerometer
    int xRead = analogRead(xPin);
    int yRead = analogRead(yPin);
    int zRead = analogRead(zPin);
    //convert read values to degrees -90 to 90 - Needed for atan2
    int xAng = map(xRead, minVal, maxVal, -90, 90);
    int yAng = map(yRead, minVal, maxVal, -90, 90);
    int zAng = map(zRead, minVal, maxVal, -90, 90);
    //Caculate 360deg values like so: atan2(-yAng, -zAng)
    //atan2 outputs the value of -π to π (radians)
    //We are then converting the radians to degrees
   /* x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
    y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);*/
    /*if (mySerial.available()) { //블루투스에서 넘어온 데이터가 있다면
    Serial.write(mySerial.read()); //시리얼모니터에 데이터를 출력
    }*/
    mySerial.print("x: ");  //블루투스를 통해 입력된 데이터 전달
    mySerial.print(xRead);  //블루투스를 통해 입력된 데이터 전달
    mySerial.print(", ");  //블루투스를 통해 입력된 데이터 전달
    mySerial.print("y: ");  //블루투스를 통해 입력된 데이터 전달
    mySerial.print(yRead);  //블루투스를 통해 입력된 데이터 전달
    mySerial.print(", ");  //블루투스를 통해 입력된 데이터 전달
    mySerial.print("z: ");  //블루투스를 통해 입력된 데이터 전달
    mySerial.println(zRead);  //블루투스를 통해 입력된 데이터 전달
    delay(1000);//just here to slow down the serial output - Easier to read
}
