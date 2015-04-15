#include <Wire.h> //I2C Arduino Library

#define address 0x1E //0011110b, I2C 7bit address of HMC5883
const int buttonPin1 = 4;
const int buttonPin2 = 6;
const int COMPASS = 1;
const int DISTANCE = 2;
const int OFF = 3;
void setup(){
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(2, OUTPUT);
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
}

void loop(){
  compass();
  distance();
  off();
}

void distance(){
clr();
while(1){
  if(checkMode(DISTANCE)) break;
  int inch = analogRead(0)/2;
  Serial.println(inch);
  if(inch<90&&inch>=50){
    if(checkMode(DISTANCE)) goto out;
    digitalWrite(2, HIGH);
    delay(80);
    if(checkMode(DISTANCE)) goto out;
    digitalWrite(2, LOW);
    delay(80);
    if(checkMode(DISTANCE)) goto out;
    digitalWrite(2, HIGH);
    delay(80);
    if(checkMode(DISTANCE)) goto out;
    digitalWrite(2, LOW);
  }
  else if(inch<50&&inch>=25){
    if(checkMode(DISTANCE)) goto out;
    analogWrite(9,255-inch);
    digitalWrite(2, HIGH);
    delay(80);
    if(checkMode(DISTANCE)) goto out;
    analogWrite(9,0);
    delay(80);
    if(checkMode(DISTANCE)) goto out;
    analogWrite(9,255-inch);
    delay(80);
    if(checkMode(DISTANCE)) goto out;
    analogWrite(9,0);
  }
  else if(inch<25){
    if(checkMode(DISTANCE)) goto out;
    analogWrite(9,255-inch);
    digitalWrite(2, HIGH);
  }
  else{
    if(checkMode(DISTANCE)) goto out;
    digitalWrite(2, LOW);
    analogWrite(9,0);
  } 
  if(checkMode(DISTANCE)) break;
  delay(500);
  if(checkMode(DISTANCE)) break;
out:;
}
}
void compass(){
clr();
while(1){
  int x,y,z; //triple axis data
  if(checkMode(COMPASS)) break;
  //Tell the HMC5883L where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
  
 
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }
  if(checkMode(COMPASS)) break;
  
  int angle = atan2((double)y,(double)x)*(180/3.1415926535)+180;
  Serial.println("  degree: ");
  Serial.println(angle);
  
  if(checkMode(COMPASS)) break;
  if(angle<10||angle>350){
    analogWrite(9,200);
  }
  else if(angle<175){
    if(checkMode(COMPASS)) break;
    analogWrite(9,180-angle);
    delay(80);
    if(checkMode(COMPASS)) break;
    analogWrite(9,0);
    delay(80);
    if(checkMode(COMPASS)) break;
    analogWrite(9,180-angle);
    delay(80);
    if(checkMode(COMPASS)) break;
    analogWrite(9,0);
    delay(80);
    if(checkMode(COMPASS)) break;
  }
  else if(angle>185){
    analogWrite(9,angle-180);
    if(checkMode(COMPASS)) break;
    delay(80);
    if(checkMode(COMPASS)) break;
    analogWrite(9,0);
    delay(240);
    if(checkMode(COMPASS)) break;
  }
  else analogWrite(9,0);
  if(checkMode(COMPASS)) break;
  delay(250);
  if(checkMode(COMPASS)) break;
}
}
void off(){
  clr();
  while(1){
    delay(80);
    if(checkMode(OFF)) break;
  }
}
void clr(){
  digitalWrite(2, LOW);
  analogWrite(9,0);
}
bool checkMode(int mode){
  int switch1 = digitalRead(buttonPin1);
  int switch2 = digitalRead(buttonPin2);
  switch(mode){
    case COMPASS:
      if((!switch1)&&switch2) return false;
      else return true;
      break;
    case DISTANCE:
      if(switch1&&(!switch2)) return false;
      else return true;
      break;
    case OFF:
      if(switch1&&switch2) return false;
      else return true;
      break;
  }
}
