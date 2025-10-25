#define BLYNK_TEMPLATE_ID "TMPL6agbyZrQX"
#define BLYNK_TEMPLATE_NAME "Smart Water Management System"
#define BLYNK_AUTH_TOKEN "XyFr_xrX9q0-cdBc4MchV55M5zmGMOum"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Pins
#define TRIG_PIN 23      // Ultrasonic Trigger
#define ECHO_PIN 22      // Ultrasonic Echo
#define PUMP_PIN 21      // LED or Pump Relay
#define TANK_HEIGHT 40.0 // in cm

// WiFi Credentials
char auth[] = "XyFr_xrX9q0-cdBc4MchV55M5zmGMOum";
char ssid[] = "Wi-Fi 5B";
char pass[] = "#xxyyzz@112233";

// Control Flags
bool manualPump = false;
bool forcePumpOff = false;
bool pumpState = false; // false = OFF, true = ON

BlynkTimer timer;

// BLYNK Virtual Pins
// V0 = Water Level Gauge
// V1 = Manual Pump ON/OFF
// V2 = Force Pump OFF

// Manual Control (V1)
BLYNK_WRITE(V1) {
  manualPump = param.asInt(); // 1 = ON, 0 = OFF

  if (!forcePumpOff) {
    if (manualPump) {
      digitalWrite(PUMP_PIN, HIGH);
      pumpState = true;
      Serial.println("Pump ON (manual)");
      Blynk.logEvent("manual_on", "Pump manually turned ON.");
    } else {
      digitalWrite(PUMP_PIN, LOW);
      pumpState = false;
      Serial.println("Pump OFF (manual)");
      Blynk.logEvent("manual_off", "Pump manually turned OFF.");
    }
  } else {
    Serial.println("Manual control ignored: Force OFF is active.");
  }
}

// Force OFF Control (V2)
BLYNK_WRITE(V2) {
  forcePumpOff = param.asInt(); // 1 = Force OFF, 0 = Normal Mode

  if (forcePumpOff) {
    digitalWrite(PUMP_PIN, LOW);
    pumpState = false;
    Serial.println("Pump forcibly turned OFF.");
    Blynk.logEvent("force_off", "Pump forcibly turned OFF.");
  } else {
    Serial.println("Force OFF disabled. Returning to normal control.");
    Blynk.logEvent("force_off_cleared", "Force OFF mode disabled.");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Ensure pump is OFF initially

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, monitorSystem); // Check every 2 sec
}

void loop() {
  Blynk.run();
  timer.run();
}

// Water Monitoring and Auto Control
void monitorSystem() {
  float level = getWaterLevel();
  Blynk.virtualWrite(V0, level);

  Serial.print("Water Level: ");
  Serial.print(level);
  Serial.println("%");

  // If Force OFF is active, skip control logic
  if (forcePumpOff) {
    if (pumpState) {
      digitalWrite(PUMP_PIN, LOW);
      pumpState = false;
      Serial.println("Pump OFF (force override)");
    }
    return;
  }

  // Automatic control only if manual is OFF
  if (!manualPump) {
    if (!pumpState && level < 20.0) {
      digitalWrite(PUMP_PIN, HIGH);
      pumpState = true;
      Serial.println("Pump ON (auto)");
      Blynk.logEvent("low_water_level", "Water level is below 20%. Pump started automatically.");
    } else if (pumpState && level >= 90.0) {
      digitalWrite(PUMP_PIN, LOW);
      pumpState = false;
      Serial.println("Pump OFF (auto)");
      Blynk.logEvent("tank_full", "Tank is full. Pump turned off automatically.");
    }
  }
}

// Measure Water Level Percentage
float getWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  float distance = duration * 0.034 / 2.0;
  float height = TANK_HEIGHT - distance;
  float percent = (height / TANK_HEIGHT) * 100.0;

  return constrain(percent, 0.0, 100.0);
}
