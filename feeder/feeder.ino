#include <Servo.h>
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

const int SERVO_PIN = 13;
const int OPEN_POSITION = 120;
const int CLOSED_POSITION = 20;
Servo servo;

void setup(){
  Wire.begin();
  Serial.begin(9600);

  servo.attach(SERVO_PIN);
  pinMode(SERVO_PIN, OUTPUT);
  servo.write(CLOSED_POSITION);
  
//  setDS3231time(7,  // day of week
//                4,  // month
//                7,  // day
//                18, // year
//                17, // hour
//                21, // minute
//                0  // seconds
//                );
}

void loop(){
  displayTime();
  delay(1000);
}

void checkForFeeding() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

  int feedDelay = 3000; // 5 minutes

  if (hour == 8 && minute == 0) {
    // morning feeding
    feed();
  } else if (hour == 14 && minute == 0) {
    // afternoon;
    feed();
  } else if (hour == 20 && minute == 0) {
    // evening;
    feed();
  }
}

void feed() {
  servo.write(120);
  delay(500);
  servo.write(0);
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

void setDS3231time(byte dayOfWeek, byte month, byte dayOfMonth, byte year, byte hour, byte
minute, byte second){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime(){
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

  switch(dayOfWeek){
  case 1:
    Serial.print("Sunday");
    break;
  case 2:
    Serial.print("Monday");
    break;
  case 3:
    Serial.print("Tuesday");
    break;
  case 4:
    Serial.print("Wednesday");
    break;
  case 5:
    Serial.print("Thursday");
    break;
  case 6:
    Serial.print("Friday");
    break;
  case 7:
    Serial.print("Saturday");
    break;
  }
  Serial.print(" ");

  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" ");
  
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10){
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10){
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.println("");
}

