int E1 = 5;
int M1 = 4;
int E2 = 6;
int M2 = 7;
int speed = 100;
int incomingByte = 0; //for incoming serial data

void setup()
{
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  Serial.begin(9600); // 9600 bps
}

void loop()
{
  if ( Serial.available() > 0)
  {
    incomingByte = Serial.read();
    if ('f' == incomingByte) {
      goForward();
    }
    if ('g' == incomingByte) {
      goForwardFast();
    }
    if ('b' == incomingByte) {
      goBackward();
    }
    if ('s' == incomingByte) {
      stopMoto();
    }
    if ('r' == incomingByte) {
      turnRight();
    }
    if ('l' == incomingByte) {
      turnLeft();
    }
  }
}

  void goForward()
  {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, speed);
    analogWrite(E2, speed);
    delay(1000);
  }

  void goForwardFast()
  {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    digitalWrite(E1, HIGH);
    digitalWrite(E2, HIGH);
    delay(1000);
  }
  
  void goBackward()
  {
    digitalWrite(M1, LOW);
    digitalWrite(M2, LOW);
    digitalWrite(E1, HIGH);
    digitalWrite(E2, HIGH);
    delay(500);
    stopMoto();
  }

  void stopMoto()
  {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 0);
    analogWrite(E2, 0);
    delay(1000);
  }

  void turnLeft()
  {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    digitalWrite(E1, HIGH);
    digitalWrite(E2, HIGH);
    delay(500);
    stopMoto();
  }

  void turnRight()
  {
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    digitalWrite(E1, HIGH);
    digitalWrite(E2, HIGH);
    delay(500);
    stopMoto();
  }
  
