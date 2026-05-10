#define BLYNK_TEMPLATE_ID "TMPL6Ti3fnMkW"
#define BLYNK_TEMPLATE_NAME "Fire and gas detection system"
#define BLYNK_AUTH_TOKEN "-0IiWAhioEO7VaZYA1iTQu2bG1U_TJzZ"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "STARK-INDUSTRIES";
char pass[] = "friday85";

#define MQ2_AOUT_PIN 32   // Gas sensor analog output pin
#define FLAME_PIN 35      // Flame sensor pin (digital)
#define BUZZER_PIN 25     // Buzzer pin
#define GAS_LED_PIN 26    // LED for Gas Detection
#define FLAME_LED_PIN 27  // LED for Flame Detection

BlynkTimer timer;
unsigned long lastGasAlert = 0;
unsigned long lastFlameAlert = 0;
const unsigned long alertCooldown = 7000; // alert cooldown time

// Gas threshold (adjust this after testing)
int gasThreshold = 2400; // Value between 0–4095 (ESP32 ADC resolution)

// Functions for buzzer (passive buzzer with tone)
void playAlarm() {
  tone(BUZZER_PIN, 2000); // Start tone at 2kHz
}

void stopAlarm() {
  noTone(BUZZER_PIN);     // Stop tone
}

void checkSensors() {
  int flameValue = digitalRead(FLAME_PIN);
  int gasValue = analogRead(MQ2_AOUT_PIN);

  bool gasDetected = (gasValue > gasThreshold);
  bool flameDetected = (flameValue == LOW);

  // Gas Detection
  if (gasDetected) {
    if (millis() - lastGasAlert > alertCooldown) {
      Blynk.logEvent("gas_alert", "⚠️ Gas Detected!");
      Serial.println(">>> Blynk gas_alert event sent");
      lastGasAlert = millis();
    }
    Serial.println("🚨 GAS DETECTED! Value: " + String(gasValue));
    digitalWrite(GAS_LED_PIN, HIGH);
  } else {
    Serial.println("✅ No Gas Detected. Value: " + String(gasValue));
    digitalWrite(GAS_LED_PIN, LOW);
  }

  // Flame Detection
  if (flameDetected) {
    if (millis() - lastFlameAlert > alertCooldown) {
      Blynk.logEvent("fire_alert", "🔥 Fire Detected!");
      Serial.println(">>> Blynk fire_alert event sent");
      lastFlameAlert = millis();
    }
    Serial.println("🔥 FLAME DETECTED!");
    digitalWrite(FLAME_LED_PIN, HIGH);
  } else {
    Serial.println("✅ No Flame Detected");
    digitalWrite(FLAME_LED_PIN, LOW);
  }

  // Buzzer
  if (gasDetected || flameDetected) {
    playAlarm();
  } else {
    stopAlarm();
  }

  Serial.println("---");
}

void setup() {
  Serial.begin(115200);

  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GAS_LED_PIN, OUTPUT);
  pinMode(FLAME_LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW); 
  digitalWrite(GAS_LED_PIN, LOW);
  digitalWrite(FLAME_LED_PIN, LOW);

  Serial.println("Gas & Flame Detection System Ready!");
  Serial.println("MQ2 set to ANALOG input");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(3000L, checkSensors);
}

void loop() {
  Blynk.run();
  timer.run();
}
