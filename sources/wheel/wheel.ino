/**************************************************************************************************************************
   Operater-Bot
   wheel.ino
   Last Modified - 2017/5/14
   by nicholaslu
   Control Motos and direction
   Remove double slash to let gofunctions to decide running time

    Arduino GND -> GY271/HMC5883L GND
    Arduino 3.3V -> GY271/HMC5883L VCC
    Arduino A4 (SDA) -> GY271/HMC5883L SDA
    Arduino A5 (SCL) -> GY271/HMC5883L SCL

    Reference:  https://www.dipmicro.com/store/GY271-MOD

**************************************************************************************************************************/
#include <Wire.h> //I2C Arduino Library
#include <Servo.h>

#define HMC5883L_ADDR 0x1E //0011110b, I2C 7bit address of HMC5883
#define ECHOPIN 12 // Sonar 
#define TRIGPIN 13

Servo snServo; // create servo object

// L298P
// speed is the default PWM speed
int E1 = 5;
int M1 = 4;
int E2 = 6;
int M2 = 7;
int speed = 100;
int incomingByte = 0; // for incoming serial data
int i = 1;
bool haveHMC5883L = false;

void setup()
{
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  snServo.attach(11); // attaches the servo on pin 11 to the servo object
  snServo.write(0);
  delay(500);
  snServo.write(180);
  delay(500);
  snServo.write(90);
  Serial.begin(9600); // 9600 bps
  Wire.begin();
  // lower I2C clock
  TWBR = 78;  // 25 kHz
  TWSR |= _BV (TWPS0);  // change prescaler
}

void loop()
{
  if ( Serial.available() > 0)
  {
    incomingByte = Serial.read();
    if ('f' == incomingByte) {
      goForward(1000);
    }
    if ('g' == incomingByte) {
      goForwardFast(1000);
    }
    if ('b' == incomingByte) {
      goBackward(1000);
    }
    if ('e' == incomingByte) {
      goBackwardFast(1000);
    }
    if ('s' == incomingByte) {
      stopMoto(1000);
    }
    if ('r' == incomingByte) {
      turnRight(500);
    }
    if ('l' == incomingByte) {
      turnLeft(500);
    }
  }
  /*
    getDistance();
    getHead();
  */
  if (i == 1) walk();
  i = 0;
  stopMoto(1);
}

void goForward(int i)
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, speed);
  analogWrite(E2, speed);
  //delay(i);
  //stopMoto();
}

void goForwardFast(int i)
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  digitalWrite(E1, HIGH);
  digitalWrite(E2, HIGH);
  //delay(i);
  //stopMoto();
}

void goBackward(int i)
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  analogWrite(E1, speed);
  analogWrite(E2, speed);
  //delay(i);
  //stopMoto();
}

void goBackwardFast(int i)
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  digitalWrite(E1, HIGH);
  digitalWrite(E2, HIGH);
  //delay(i);
  //stopMoto();
}

void stopMoto(int i)
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 0);
  analogWrite(E2, 0);
  //delay(i);
  //stopMoto();
}

// According to my test, it gonna turn almost 90 degrees while i = 500m
void turnLeft(int i)
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  digitalWrite(E1, HIGH);
  digitalWrite(E2, HIGH);
  //delay(i);
  //stopMoto();
}

void turnRight(int i)
{
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  digitalWrite(E1, HIGH);
  digitalWrite(E2, HIGH);
  //delay(i);
  //stopMoto();
}

float getHead()
{
  float angle;
  bool detect = detectHMC5883L();

  if (!haveHMC5883L)
  {
    if (detect)
    {
      haveHMC5883L = true;
      Serial.println("HMC5883L detected");
      // Put the HMC5883 IC into the correct operating mode
      Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
      Wire.write(0x02); //select mode register
      Wire.write(0x00); //continuous measurement mode
      Wire.endTransmission();
    }
    else
    {
      Serial.println("HMC5883L not detected");
      delay(2000);
      return;
    }
  }
  else
  {
    if (!detect) {
      haveHMC5883L = false;
      Serial.println("HMC5883L lost");
      delay(2000);
      return;
    }
  }

  int x, y, z; //triple axis data

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

  //Read data from each axis, 2 registers per axis
  Wire.requestFrom(HMC5883L_ADDR, 6);
  if (6 <= Wire.available()) {
    x = Wire.read() << 8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read() << 8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read() << 8; //Y msb
    y |= Wire.read(); //Y lsb
  }

  //Print out values of each axis
  angle = atan2(y , x) * 180 / PI;
  Serial.println(angle);
  //  Serial.print("x: ");
  //  Serial.print(x);
  //  Serial.print("  y: ");
  //  Serial.print(y);
  //  Serial.print("  z: ");
  //  Serial.println(z);
  return angle;
}

bool detectHMC5883L()
{
  // read identification registers
  Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
  Wire.write(10); //select Identification register A
  Wire.endTransmission();
  Wire.requestFrom(HMC5883L_ADDR, 3);
  if (3 == Wire.available()) {
    char a = Wire.read();
    char b = Wire.read();
    char c = Wire.read();
    if (a == 'H' && b == '4' && c == '3')
      return true;
  }

  return false;
}

float getDistance()
{
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  float distance = pulseIn(ECHOPIN, HIGH);
  distance = distance / 58;
  Serial.println(distance);
  delay(200);
  if (distance > 100) {
    digitalWrite(13, HIGH);
    delay(100);
  }
  else {
    digitalWrite(13, LOW);
    delay(100);
  }
  return distance;
}

void walk()
{
  float curHead, oriHead, curDist, deviation;
  float guess = 10;
  oriHead = getHead();
  curDist = getDistance();
  delay(100);
  while (curDist > 1000) {
    curDist = getDistance();
    delay(100);
  }
  while (curDist >= 15) {
    curHead = getHead();
    deviation = abs(curHead) - abs(oriHead);
    if (deviation <= 10) {
      goForward(1);
    }
    else {
      stopMoto(1);
      while (abs(curHead) - abs(oriHead) > 10)
      {
        if (guess > 0) {
          turnRight(1);
          delay(abs(guess) * 10);
        }
        if (guess < 0) {
          turnLeft(1);
          delay(abs(guess) * 10);
        }
        curHead = getHead();
        if (abs(curHead) - abs(oriHead) >= deviation) {
          guess = -guess;
        }
        else {
          guess = guess / 2;
        }
      }
    }
    curDist = getDistance();
  }
  stopMoto(1);
}

