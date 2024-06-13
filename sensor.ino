#include <SoftwareSerial.h>
#include <DHT.h>

#define PIN_DUST_OUT A0   // 미세먼지 센서 데이터 출력
#define PIN_DUST_LED 11   // 미세먼지 센서 적외선 LED
#define PIN_DHT_OUT 7     // DHT11 센서 데이터 출력
#define PIN_LED_R 10      // RGB LED 적색핀
#define PIN_LED_G 9       // RGB LED 녹색핀
#define PIN_RAIN_OUT A1   // Rain sensor data output
#define PIN_GAS_OUT A2    // 가스 센서 데이터 출력
#define PIN_LIGHT_OUT A3  // 조도 센서 데이터 출력

// DHT11 센서 설정
DHT dht(PIN_DHT_OUT, DHT11);

// Bluetooth settings
SoftwareSerial BTSerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(PIN_DUST_LED, OUTPUT);
  analogWrite(PIN_LED_G, 0);         // 녹색 LED 초기값 설정
  analogWrite(PIN_LED_R, 0);         // 적색 LED 초기값 설정
  digitalWrite(PIN_DUST_LED, HIGH);  // 미세먼지 센서 적외선 LED 초기값 설정(부논리)
  dht.begin();
}

float read_dust() {
  digitalWrite(PIN_DUST_LED, LOW);
  delayMicroseconds(280);
  int dustval = analogRead(PIN_DUST_OUT);
  delayMicroseconds(40);
  digitalWrite(PIN_DUST_LED, HIGH);
  delayMicroseconds(9680);
  float voltage = dustval * (5.0 / 1024.0);
  float dust_ug = (0.17 * voltage - 0.1) * 1000;
  return dust_ug >= 0 ? dust_ug : 0;
}

float read_rain() {
  int rainval = analogRead(PIN_RAIN_OUT);

  return rainval;
}

float read_gas() {
  int gasval = analogRead(PIN_GAS_OUT);
  return gasval;
}

float read_light() {
  int lightval = analogRead(PIN_LIGHT_OUT);
  return lightval;
}

void loop() {
  float dust_ug = read_dust();                  // 미세먼지 데이터 수집
  float dht_temperature = dht.readTemperature();  // DHT11 온도 데이터 수집
  float dht_humidity = dht.readHumidity();        // DHT11 습도 데이터 수집
  float rain_level = read_rain();                 // 비 센서 데이터 수집
  float gas_level = read_gas();                   // 가스 센서 데이터 수집
  float light_level = read_light();               // 조도 센서 데이터 수집

  Serial.print("Temperature: ");
  Serial.print(dht_temperature);
  Serial.print(" C\nHumidity: ");
  Serial.print(dht_humidity);
  Serial.print(" %\nDust: ");
  Serial.print(dust_ug);
  Serial.print(" ug\nRain Level: ");
  Serial.print(rain_level);
  Serial.print(" V\nGas Level: ");
  Serial.print(gas_level);
  Serial.print(" V\nLight Level: ");
  Serial.print(light_level);
  Serial.println("");

  // 시리얼로 데이터 전송
  Serial.print(dht_temperature);
  Serial.print(",");
  Serial.print(dht_humidity);
  Serial.print(",");
  Serial.print(dust_ug);
  Serial.print(",");
  Serial.print(rain_level);
  Serial.print(",");
  Serial.print(gas_level);
  Serial.print(",");
  Serial.println(light_level);

  // Send data via Bluetooth
  BTSerial.print(dht_temperature);
  BTSerial.print(",");
  BTSerial.print(dht_humidity);
  BTSerial.print(",");
  BTSerial.print(dust_ug);
  BTSerial.print(",");
  BTSerial.print(rain_level);
  BTSerial.print(",");
  BTSerial.print(gas_level);
  BTSerial.print(",");
  BTSerial.println(light_level);

  delay(5000);

  if(dust_ug < 35) {         // 미세먼지가 35ug미만일 경우
    analogWrite(PIN_LED_G, 20);
    analogWrite(PIN_LED_R, 0);
  } else if(dust_ug < 75) {  // 미세먼지가 35ug이상, 75ug미만일 경우
    analogWrite(PIN_LED_G, 30);
    analogWrite(PIN_LED_R, 20);
  } else {                   // 미세먼지가 75ug 이상일 경우
    analogWrite(PIN_LED_G, 0);
    analogWrite(PIN_LED_R, 20);
  }
}