#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class BLEManager
{
  public:

    class ServerCallback: public BLEServerCallbacks {
      public:
        ServerCallback(BLEManager * manager) : manager(manager) {}
        BLEManager * manager;

        void onConnect(BLEServer* pServer) {
          manager->deviceConnected = true;
          DBG("BLE is connected.");
        };

        void onDisconnect(BLEServer* pServer) {
          manager->deviceConnected = false;
          DBG("BLE is disconnected.");
        }
    };

    class DataCallback: public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pCharacteristic) {

          String message = String(pCharacteristic->getValue().c_str());
          DBG("[BLE] Received " + message);
          SerialManager::instance->parseMessage(message);
        }
    };


    BLEManager() {}

    BLEServer *pServer = NULL;
    BLECharacteristic * pTxCharacteristic;
    bool deviceConnected = false;
    bool oldDeviceConnected = false;
    uint8_t txValue = 0;

    bool isActivated;
    
    void init()
    {
      // Create the BLE Device

      isActivated = Config::instance->getBLEMode();

      if(!isActivated)
      {
        DBG("BLE not activated in preferences, not initializing");
        return;
      }
      
      String bleName ="FlowConnect "+Config::instance->getDeviceName();
      BLEDevice::init(bleName.c_str());

      // Create the BLE Server
      pServer = BLEDevice::createServer();
      pServer->setCallbacks(new ServerCallback(this));

      // Create the BLE Service
      BLEService *pService = pServer->createService(SERVICE_UUID);

      // Create a BLE Characteristic
      pTxCharacteristic = pService->createCharacteristic(
                            CHARACTERISTIC_UUID_TX,
                            BLECharacteristic::PROPERTY_NOTIFY
                          );

      pTxCharacteristic->addDescriptor(new BLE2902());

      BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
          CHARACTERISTIC_UUID_RX,
          BLECharacteristic::PROPERTY_WRITE
                                              );

      pRxCharacteristic->setCallbacks(new DataCallback());

      // Start the service
      pService->start();

      // Start advertising
      pServer->getAdvertising()->start();
      DBG("BLE is init with name "+bleName);
    }

    void update()
    {
      if(!isActivated) return;
      
      if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
      }
      // connecting
      if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
      }
    }
};
