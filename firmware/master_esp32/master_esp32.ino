#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

String targetName = "ESP32_Robot_Slave";
bool connected = false;

// Joysticks (Ακριβώς όπως στον αρχικό κώδικα)
const int JOY_Y_PIN = 34; // Γκάζι - Εμπρός/Πίσω 
const int JOY_X_PIN = 32; // Τιμόνι - Αριστερά/Δεξιά 

void setup() {
  Serial.begin(9600); // Ίδιο baud rate με τον αρχικό σου κώδικα
  
  SerialBT.begin("ESP32_Robot_Master", true); 
  Serial.println("Αναζήτηση Slave...");

  connected = SerialBT.connect(targetName);
  
  if (connected) {
    Serial.println("Το σύστημα ξεκίνησε! Αναμονή για κίνηση joystick...");
  } else {
    Serial.println("Η σύνδεση απέτυχε. Επανεκκίνηση...");
    ESP.restart();
  }
}

void loop() {
  if (connected) {
    int joyY = analogRead(JOY_Y_PIN);
    int joyX = analogRead(JOY_X_PIN);

    // Αποστολή πακέτου: "joyY,joyX\n"
    SerialBT.print(joyY);
    SerialBT.print(",");
    SerialBT.println(joyX);
  }
  
  delay(20); // Ακριβώς 20ms όπως στον αρχικό σου κώδικα!
}