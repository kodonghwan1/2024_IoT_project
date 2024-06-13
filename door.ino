int motorIA = 5;
int motorIB = 6;

void setup() {
  Serial.begin(9600);
  pinMode(motorIA, OUTPUT);
  pinMode(motorIB, OUTPUT);
 
  // 모터를 초기 상태로 멈춤
  digitalWrite(motorIA, LOW);
  digitalWrite(motorIB, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'c') { // 'c' 명령어로 닫기
      closeMotor();
    } else if (command == 'o') { // 'o' 명령어로 열기
      openMotor();
    } else if (command == 's') { // 's' 명령어로 정지
      stopMotor();
    }
  }
}

void openMotor() {
  digitalWrite(motorIA, HIGH);
  digitalWrite(motorIB, LOW);
}

void closeMotor() {
  digitalWrite(motorIA, LOW);
  digitalWrite(motorIB, HIGH);
}

void stopMotor() {
  digitalWrite(motorIA, LOW);
  digitalWrite(motorIB, LOW);
}
