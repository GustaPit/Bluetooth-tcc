#include <BLEDevice.h>    // Biblioteca principal para inicializar e gerenciar o dispositivo BLE
#include <BLEUtils.h>     // Inclui utilitários para manipular características, serviços e UUIDs BLE
#include <BLEServer.h>    // Permite criar um servidor BLE no ESP32

// ==== PINOS DO PROJETO ====
#define PIN_LED_R 13   // Pino do LED RGB - Vermelho
#define PIN_LED_G 12   // Pino do LED RGB - Verde
#define PIN_LED_B 14   // Pino do LED RGB - Azul
#define PIN_BUZZER 27  // Pino do buzzer passivo
#define PIN_MOTOR 26   // Pino do motor

bool sinalRecebido = false; // Guarda o estado do comando (ON ou OFF)

// Nome do dispositivo BLE que aparecerá no celular
#define DEVICE_NAME "ESP32_BLE"

// UUIDs para o serviço e característica BLE
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-ba0987654321"

// Ponteiros para o servidor e característica BLE
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

// Classe de callback para tratar comandos recebidos via BLE
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
        String valor = pChar->getValue().c_str();  // Lê o valor recebido do celular
        valor.toUpperCase();  //serve para caso o usuário digitar as palavras chave em formas diferentes, ainda rodar o código

        if (valor == "ON") {
            sinalRecebido = true;
            Serial.println("Comando: ON - Dispositivos ligados");

            // Liga buzzer (1kHz)
            ledcWriteTone(PIN_BUZZER, 1000);
        } 
        else if (valor == "OFF") {
            sinalRecebido = false;
            Serial.println("Comando: OFF - Dispositivos desligados");

            // Para o buzzer
            ledcWriteTone(PIN_BUZZER, 0);
        }
    }
};

void setup() {
  Serial.begin(115200); // Inicializa comunicação serial para debug

  // Configura os pinos de saída
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  // === Configuração do PWM para o buzzer
  // No core 3.3.0, usa-se ledcAttach(pin, freq, resolution)
  ledcAttach(PIN_BUZZER, 1000, 8); // Pino, freq inicial, resolução (8 bits)

  // Inicializa BLE
  BLEDevice::init(DEVICE_NAME);  // Define o nome do dispositivo
  pServer = BLEDevice::createServer(); // Cria servidor BLE

  BLEService *pService = pServer->createService(SERVICE_UUID);   // Cria um serviço com o UUID definido
  pCharacteristic = pService->createCharacteristic(  
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
                    );
  pCharacteristic->setCallbacks(new MyCallbacks()); // Associa os callbacks de leitura/escrita  
  pCharacteristic->setValue("OFF"); // Valor inicial

  pService->start(); // Inicia o serviço

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); // Configura anúncio do BLE
  pAdvertising->addServiceUUID(SERVICE_UUID);    // Anuncia o serviço
  pAdvertising->setScanResponse(true); // Responde a scans de dispositivos
  pAdvertising->start(); // Inicia publicidade BLE

  Serial.println("BLE iniciado. Conecte-se e envie 'ON' ou 'OFF'");
}

void loop() {
  if (sinalRecebido) { //Se sinal recebido for 'true', então executará todas as linhas de código dentro do if
    // LED RGB em roxo (led anodo comum)
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, LOW);

    // Liga motor
    digitalWrite(PIN_MOTOR, HIGH);

  } else { // se não, se o sinalRecebido for 'false', desligará (ou permanecerá desligado) o bracelete, e executará os códigos abaixo
    // Desliga LED RGB
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_B, HIGH);

    // Desliga motor
    digitalWrite(PIN_MOTOR, LOW);
  }
}
