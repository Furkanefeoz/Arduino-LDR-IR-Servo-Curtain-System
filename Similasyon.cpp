#include <Servo.h>

const int LDR_PIN = A0;
const int SERVO_PIN = 9;
const int LED_PIN = 13;

// Eşikler (senin istediğin)
const int E1 = 930;
const int E2 = 950;

// Servo değerleri (kalibrasyon gerekebilir)
int STOP_VAL  = 90;    // burayı kalibrasyondan gelen değere ayarla
int RIGHT_VAL = STOP_VAL + 20;
int LEFT_VAL  = STOP_VAL - 20;

// step süresi (15° yaklaşık)
const int STEP_MS = 180;

Servo s;

bool passedE1 = false;
bool passedE2 = false;

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  s.attach(SERVO_PIN);
  s.write(STOP_VAL);
  delay(200);
  Serial.println("Diagnostik basladi. LDR okumasini degistir ve izleyin.");
}

void loop() {
  int v = analogRead(LDR_PIN);

  // Seri çıktı çok sık olmasın, 200ms aralıklı yazdır.
  if (millis() - lastPrint > 200) {
    Serial.print("LDR = ");
    Serial.println(v);
    lastPrint = millis();
  }

  // --- E1 ---
  if (v > E1 && !passedE1) {
    Serial.println("E1 asildi -> +15deg (simule)");
    digitalWrite(LED_PIN, HIGH);
    doStepRight();
    digitalWrite(LED_PIN, LOW);
    passedE1 = true;
  }
  if (v < E1 && passedE1) {
    Serial.println("E1 altina dustu -> -15deg (geri)");
    digitalWrite(LED_PIN, HIGH);
    doStepLeft();
    digitalWrite(LED_PIN, LOW);
    passedE1 = false;
  }

  // --- E2 ---
  if (v > E2 && !passedE2) {
    Serial.println("E2 asildi -> +15deg (simule)");
    digitalWrite(LED_PIN, HIGH);
    doStepRight();
    digitalWrite(LED_PIN, LOW);
    passedE2 = true;
  }
  if (v < E2 && passedE2) {
    Serial.println("E2 altina dustu -> -15deg (geri)");
    digitalWrite(LED_PIN, HIGH);
    doStepLeft();
    digitalWrite(LED_PIN, LOW);
    passedE2 = false;
  }

  delay(30);
}

void doStepRight() {
  // servo bağlı değilse bile LED göreceksin
  s.write(RIGHT_VAL);
  delay(STEP_MS);
  s.write(STOP_VAL);
  delay(80);
}

void doStepLeft() {
  s.write(LEFT_VAL);
  delay(STEP_MS);
  s.write(STOP_VAL);
  delay(80);
}
