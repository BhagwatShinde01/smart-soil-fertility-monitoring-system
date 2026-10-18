/*******************************
 *  LCD + NPK + DHT + MOISTURE
 *  Blynk OPTIONAL (Non-blocking)
 *******************************/

#define BLYNK_TEMPLATE_ID "Your_blynk_templete"
#define BLYNK_TEMPLATE_NAME "Template_name"
#define BLYNK_DEVICE_NAME "Device_name"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HardwareSerial.h>

BlynkTimer timer;

// ===== USER CONFIG =====
char auth[] = "Authentication_code";
const char* ssid = "ssid";
const char* pass = "passward";

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== DHT11 =====
#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== Moisture =====
#define PIN_MOIST 32
#define MOISTURE_WET_ADC 3044
#define MOISTURE_DRY_ADC 4095

// ===== RS485 NPK =====
HardwareSerial Modbus(2);
#define RE_DE 4

// ===== READ MODBUS REGISTER =====
uint16_t readReg(uint16_t reg) {
  uint8_t req[8] = {
    0x01, 0x03,
    (uint8_t)(reg >> 8),
    (uint8_t)(reg & 0xFF),
    0x00, 0x01,
    0x00, 0x00
  };

  uint16_t crc = 0xFFFF;
  for (int i = 0; i < 6; i++) {
    crc ^= req[i];
    for (int j = 0; j < 8; j++)
      crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
  }
  req[6] = crc & 0xFF;
  req[7] = crc >> 8;

  digitalWrite(RE_DE, HIGH);
  Modbus.write(req, 8);
  Modbus.flush();
  digitalWrite(RE_DE, LOW);

  delay(50);

  uint8_t resp[7];
  if (Modbus.readBytes(resp, 7) != 7) return 0;

  return (resp[3] << 8) | resp[4];
}

// ===== Moisture =====
int readMoisture() {
  int raw = analogRead(PIN_MOIST);
  raw = constrain(raw, MOISTURE_WET_ADC, MOISTURE_DRY_ADC);
  int percent = map(raw, MOISTURE_DRY_ADC, MOISTURE_WET_ADC, 0, 100);
  return constrain(percent, 0, 100);
}

// ===== Health Score =====
int computeHealth(int N, int P, int K, int moist) {
  int score = 0;
  if (N >= 30 && N <= 80) score += 25;
  if (P >= 20 && P <= 60) score += 25;
  if (K >= 10 && K <= 50) score += 25;
  if (moist >= 30 && moist <= 70) score += 25;
  return score;
}

// ===== LCD Screens =====
int screen = 1;

void updateLCD(float t, float h, int moist, int N, int P, int K, int health) {
  lcd.clear();

  if (screen == 1) {
    lcd.setCursor(0,0);
    lcd.print("T:"); lcd.print(t,1); lcd.print("C H:");
    lcd.print(h,0);
    lcd.setCursor(0,1);
    lcd.print("Moist: "); lcd.print(moist); lcd.print("%");
  }
  else if (screen == 2) {
    lcd.setCursor(0,0);
    lcd.print("N:"); lcd.print(N); lcd.print(" P:"); lcd.print(P);
    lcd.setCursor(0,1);
    lcd.print("K:"); lcd.print(K);
  }
  else if (screen == 3) {
    lcd.setCursor(0,0);
    lcd.print("Health Score: ");
    lcd.print(health);
    lcd.setCursor(0,1);
    if (health >= 75) lcd.print("Status: Good");
    else if (health >= 40) lcd.print("Status: OK");
    else lcd.print("Status: Bad");
  }

  screen++;
  if (screen > 3) screen = 1;
}

// ===== MAIN SENSOR TASK =====
void sendAllData() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  int moist  = readMoisture();

  int N = readReg(0x001E);
  int P = readReg(0x001F);
  int K = readReg(0x0020) / 10;

  int health = computeHealth(N, P, K, moist);

  updateLCD(temp, hum, moist, N, P, K, health);

  // Send to Blynk ONLY if connected
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.virtualWrite(V0, temp);
    Blynk.virtualWrite(V1, hum);
    Blynk.virtualWrite(V2, moist);
    Blynk.virtualWrite(V3, health);
    Blynk.virtualWrite(V4, N);
    Blynk.virtualWrite(V5, P);
    Blynk.virtualWrite(V6, K);
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.print("System Starting...");
  delay(1000);

  dht.begin();

  pinMode(RE_DE, OUTPUT);
  digitalWrite(RE_DE, LOW);
  Modbus.begin(4800, SERIAL_8N1, 16, 17);

  // Non-blocking WiFi + Blynk
  WiFi.begin(ssid, pass);
  Blynk.config(auth);

  lcd.clear();
  lcd.print("System Ready");

  timer.setInterval(5000L, sendAllData);
}

// ===== LOOP =====
void loop() {
  Blynk.run();   // safe even if not connected
  timer.run();
}
