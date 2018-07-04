#include <Servo.h>
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

const int BUZZER_PIN = 8;
const int SERVO_PIN = 13;

const int OPEN_POSITION = 140;
const int CLOSED_POSITION = 40;

Servo servo;

// BUZZER CONSONANTS
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;


void setup(){
  Wire.begin();
  Serial.begin(9600);

  servo.attach(SERVO_PIN);
  pinMode(SERVO_PIN, OUTPUT);
  servo.write(CLOSED_POSITION);
  //servo.detach();

  pinMode(BUZZER_PIN, OUTPUT);
  
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
  checkForFeeding();
  delay(1000);
}

void checkForFeeding() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

  if (hour == 6 && minute == 30 && second == 0) {
    // morning
    feed();
  } else if (hour == 12 && minute == 30 && second == 0) {
    // afternoon
    feed();
  } else if (hour == 19 && minute == 0 && second == 0) {
    // evening
    feed();
  }
}

void feed() {
  playMusic(); 
  
  // 500 = 30g
  // 350 = 10g
  // 250 = 7.5g
  // 200 = 2.14g
  // 100 = nothing

  float totalTime = 900000; // 15m
  float numFeedings = 14; // 14 * 200ms delay = 30g
  float feedDelay = totalTime / numFeedings;
  Serial.print("Feed delay = ");
  Serial.print(feedDelay);
  Serial.println("");
  
  for (int i=0; i < numFeedings; i++){
    Serial.print("Feed portion #");
    Serial.print(i);
    Serial.println("");
    
    feedPortion();
    delay(feedDelay);
   }
}

void feedPortion() {
  //servo.attach(SERVO_PIN);
  servo.write(OPEN_POSITION);
  delay(350);
  servo.write(CLOSED_POSITION);
  //servo.detach();
}

/*
 * BUZZER METHODS
 */

void playMusic() {
  beep(a, 500);
  beep(a, 500);    
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);  
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  delay(500);
 
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);  
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  delay(500);
}


void beep(int note, int duration) {
  //Play tone on buzzerPin
  tone(BUZZER_PIN, note, duration);

  delay(duration);
 
  //Stop tone on buzzerPin
  noTone(BUZZER_PIN);
 
  delay(50);
}

/*
 * RTC METHODS
 */

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

