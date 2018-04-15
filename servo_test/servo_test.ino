#include <Servo.h>

const int servoPin = 13;
Servo myServo;

void setup() {
  myServo.attach(servoPin);
  pinMode(servoPin, OUTPUT);
  myServo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  myServo.write(120);
  delay(500);
  //myServo.write(20);
  delay(4000);
}
