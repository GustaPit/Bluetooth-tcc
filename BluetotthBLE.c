#include <BLEDevice.h> // Biblioteca principal para inicializar e gerenciar o dispositivo BLE no ESP32
#include <BLEUtils.h> // Inclui utilitários para manipular características, serviços e UUIDs BLE
#include <BLEServer.h> // Permite criar um servidor BLE no ESP32 (o ESP32 será o periférico que recebe conexões)

// ==== PINOS DO PROJETO ====
#define PIN_LED_R 13   // Pino do LED RGB - cor Vermelha
#define PIN_LED_G 12   // Pino do LED RGB - cor Verde
#define PIN_LED_B 14   // Pino do LED RGB - cor Azul
#define PIN_BUZZER 27  // Pino do buzzer
#define PIN_MOTOR 26   // Pino do motor (ou LED simples para teste)

bool sinalRecebido = false; // Variável global que guarda o estado do comando (ON ou OFF)

// Nome do dispositivo BLE que aparecerá no celular
#define DEVICE_NAME "ESP32_BLE"

// UUIDs (identificadores únicos) para o serviço e característica BLE
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-ba0987654321"

// Ponteiros para o servidor e característica BLE
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

// Classe de callback para tratar comandos recebidos via BLE
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
        // Lê o valor escrito na característica
        // pChar->getValue() retorna std::string, então convertemos para String (Arduino)
        String valor = pChar->getValue().c_str();
        valor.toUpperCase(); // Converte o texto para maiúsculo ("on" -> "ON")

        if (valor == "ON") {
            sinalRecebido = true;  // Atualiza flag
            Serial.println("Comando: ON - Dispositivos ligados");
            tone(PIN_BUZZER, 1000); // Liga buzzer com tom de 1 kHz
        } 
        else if (valor == "OFF") {
            sinalRecebido = false; // Atualiza flag
            Serial.println("Comando: OFF - Dispositivos desligados");
            noTone(PIN_BUZZER); // Para o buzzer
        }
    }
};

void setup() {
  Serial.begin(115200);

  // Configura os pinos como saída
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  // Inicializa o BLE com o nome definido
  BLEDevice::init(DEVICE_NAME);

  // Cria um servidor BLE
  pServer = BLEDevice::createServer();

  // Cria um serviço BLE com o UUID especificado
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // Cria uma característica que pode ser escrita e lida
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
                    );

  // Define a classe de callbacks para tratar comandos recebidos
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Define valor inicial da característica como "OFF"
  pCharacteristic->setValue("OFF");

  // Inicia o serviço BLE
  pService->start();

  // Configura a propaganda BLE (broadcast para ser descoberto por apps de celular)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  Serial.println("BLE iniciado. Conecte-se e escreva 'ON' ou 'OFF'");
}

void loop() {
  // Verifica estado da variável 'sinalRecebido' e controla os dispositivos
  if (sinalRecebido) {
    // Acende LED RGB na cor verde (ajuste depende do tipo de LED, ânodo/cátodo comum)
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, LOW);

    // Liga motor (ou LED simples)
    digitalWrite(PIN_MOTOR, HIGH);
    
  } else {
    // Desliga LEDs (todos em HIGH, dependendo da ligação pode ser apagado)
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, HIGH);

    // Desliga motor
    digitalWrite(PIN_MOTOR, LOW);
  }
}
