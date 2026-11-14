#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

// --- Ορισμός των νέων "απλωμένων" Pins του ESP32 ---

// Αριστερό Μοτέρ
const int ENA = 14;
const int IN1 = 27;
const int IN2 = 25;

// Δεξί Μοτέρ
const int ENB = 22;
const int IN3 = 21;
const int IN4 = 19;

void setup() {
  Serial.begin(9600); // Ίδιο baud rate με τον αρχικό σου κώδικα
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  SerialBT.begin("ESP32_Robot_Slave"); 
  Serial.println("Το σύστημα ξεκίνησε! Αναμονή για κίνηση joystick...");
}

void loop() {
  if (SerialBT.available()) {
    String incomingData = "";
    
    // Διαβάζουμε το πιο πρόσφατο πακέτο για μηδενικό lag
    while (SerialBT.available()) {
      incomingData = SerialBT.readStringUntil('\n');
    }
    incomingData.trim();

    int commaIndex = incomingData.indexOf(',');
    if (commaIndex > 0) {
      int joyY = incomingData.substring(0, commaIndex).toInt();
      int joyX = incomingData.substring(commaIndex + 1).toInt();

      int mappedY = map(joyY, 0, 4095, -255, 255);
      int mappedX = map(joyX, 0, 4095, -255, 255);

      if (abs(mappedY) < 25) mappedY = 0;
      if (abs(mappedX) < 25) mappedX = 0;

      // Λογική: Car-Like Quadrant Differential Steering
      int V = mappedY; 
      int W = mappedX; 

      int leftMotorSpeed = 0;
      int rightMotorSpeed = 0;

      if (W > 0) { // Στροφή Δεξιά
        leftMotorSpeed = V; 
        rightMotorSpeed = V - (V * W / 255); 
      } 
      else if (W < 0) { // Στροφή Αριστερά
        rightMotorSpeed = V;
        leftMotorSpeed = V - (V * abs(W) / 255); 
      } 
      else { // Κίνηση ευθεία
        leftMotorSpeed = V;
        rightMotorSpeed = V;
      }

      driveLeftMotor(leftMotorSpeed);
      driveRightMotor(rightMotorSpeed);
    }
  }
}

// ================= Συναρτήσεις Οδήγησης =================

void driveLeftMotor(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, abs(speed));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void driveRightMotor(int speed) {
  if (speed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  } else if (speed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, abs(speed));
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}