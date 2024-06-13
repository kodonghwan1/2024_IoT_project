int cdsPin = A0;
int motorIA = 5;
int motorIB = 6;
int swClose = 8;
int swOpen = 9;

int doorState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(motorIA,OUTPUT);
  pinMode(motorIB,OUTPUT);
  pinMode(swClose,INPUT);
  pinMode(swOpen,INPUT);
}

void loop() {
  int cdsValue = analogRead(cdsPin);
  Serial.println(cdsValue);
  if(cdsValue <= 300) //밝은데
  {
    if(doorState == 0) //창문이 닫혀있으면
    {
      doorState = 1; //도어의 상태는 열림상태로 변경
      digitalWrite(motorIA,HIGH);
      digitalWrite(motorIB,LOW);
      delay(5000);
    }
  }
  else if(cdsValue >= 600) //어두운데
  {
    if(doorState == 1) //창문이 열려있으면
    {
      doorState = 0; //도어의 상태는 열림상태로 변경
      digitalWrite(motorIA,LOW);
      digitalWrite(motorIB,HIGH);
      delay(5000);
    }
  }

  
  if(digitalRead(swClose) == 0)
  {
    digitalWrite(motorIA,HIGH);
    digitalWrite(motorIB,LOW);
  }
  else if(digitalRead(swOpen) == 0)
  {
    digitalWrite(motorIA,LOW);
    digitalWrite(motorIB,HIGH);

  }
  else
  {
    digitalWrite(motorIA,LOW);
    digitalWrite(motorIB,LOW);
  }
}
