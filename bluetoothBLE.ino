#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Pin assignments
#define DEVICE1_PIN 18
#define DEVICE2_PIN 19

// BLE setup
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// UUIDs (you can generate your own if needed)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// === BLE Callback for connection status ===
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected.");
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected.");
    }
};

// === BLE Callback for receiving commands ===
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // Use Arduino String directly
      String command = pCharacteristic->getValue().c_str(); // converts std::string to String
      command.trim();

      if (command.length() > 0) {
        Serial.print("Received: ");
        Serial.println(command);

        if (command == "ON1") {
          digitalWrite(DEVICE1_PIN, HIGH);
          Serial.println("Device 1 ON");
          pCharacteristic->setValue("D18 ON");
          pCharacteristic->notify();
        } 
        else if (command == "OFF1") {
          digitalWrite(DEVICE1_PIN, LOW);
          Serial.println("Device 1 OFF");
          pCharacteristic->setValue("D18 OFF");
          pCharacteristic->notify();
        } 
        else if (command == "ON2") {
          digitalWrite(DEVICE2_PIN, HIGH);
          Serial.println("Device 2 ON");
          pCharacteristic->setValue("D19 ON");
          pCharacteristic->notify();
        } 
        else if (command == "OFF2") {
          digitalWrite(DEVICE2_PIN, LOW);
          Serial.println("Device 2 OFF");
          pCharacteristic->setValue("D19 OFF");
          pCharacteristic->notify();
        } 
        else {
          Serial.println("Invalid command received");
          pCharacteristic->setValue("Invalid CMD");
          pCharacteristic->notify();
        }
      }
    }
};

void setup() {
  Serial.begin(115200);

  pinMode(DEVICE1_PIN, OUTPUT);
  pinMode(DEVICE2_PIN, OUTPUT);
  digitalWrite(DEVICE1_PIN, LOW);
  digitalWrite(DEVICE2_PIN, LOW);

  // Initialize BLE
  BLEDevice::init("ESP32_Control_BLE");

  // Create server and callbacks
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE characteristic (read + write + notify)
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Add descriptor so notifications are properly registered
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();

  Serial.println("Waiting for client connection to send commands...");
}

void loop() {
  // Reconnect advertising if disconnected
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restarted advertising");
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}


