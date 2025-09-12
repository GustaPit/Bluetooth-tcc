#include "BluetoothSerial.h"

// ==== PINOS DO PROJETO ====
#define PIN_LED_R 13
#define PIN_LED_G 12
#define PIN_LED_B 14
#define PIN_BUZZER 27
#define PIN_MOTOR 26  // LED simples no lugar do motor

bool sinalRecebido = false;

BluetoothSerial SerialBT; // objeto Bluetooth

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // nome do dispositivo Bluetooth Classic
  Serial.println("Bluetooth iniciado. Conecte-se ao 'ESP32_BT' e envie 'ON' ou 'OFF'");
  SerialBT.println("Bluetooth iniciado. Conecte-se ao 'ESP32_BT' e envie 'ON' ou 'OFF'");

  // Configura pinos
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

}

void loop() {
  // Verifica se há comando via Bluetooth
  if (SerialBT.available()) {
    String comando = SerialBT.readStringUntil('\n');
    comando.trim();
    comando.toUpperCase();

    if (comando == "ON") {
      sinalRecebido = true;
      Serial.println("Comando: ON - Dispositivos ligados");
      SerialBT.println("Comando: ON - Dispositivos ligados");

      // Liga o buzzer permanentemente
      tone(PIN_BUZZER, 1000); // 1 kHz
    } 
    else if (comando == "OFF") {
      sinalRecebido = false;
      Serial.println("Comando: OFF - Dispositivos desligados");
       SerialBT.println("Comando: OFF - Dispositivos desligados");

      // Desliga o buzzer
      noTone(PIN_BUZZER);
    }
  }

  // Controle dos dispositivos
  if (sinalRecebido) {
    // LED RGB (ânodo comum: LOW acende, HIGH apaga)
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH);  // acende verde
    digitalWrite(PIN_LED_B, LOW);

    // MOTOR de vibração ligado
    digitalWrite(PIN_MOTOR, HIGH);
  } 
  else {
    // Desliga todos os LEDs e o Motor de vibração
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_MOTOR, LOW);
  } 
}
