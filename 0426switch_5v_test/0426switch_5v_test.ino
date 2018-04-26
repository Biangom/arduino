
int pushButton = 4;

void setup() {
  Serial.begin(9600);
  pinMode(pushButton, INPUT_PULLUP); //INPUT_PULLUP모드 적용
}

void loop() {
  int buttonState = digitalRead(pushButton);
  Serial.println(buttonState);
  delay(1);    
}
