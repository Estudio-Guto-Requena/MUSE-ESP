#include <Arduino.h>
#include "BLEDevice.h"

#define MUSE_ACCELEROMETER_SCALE_FACTOR 0.0000610352
#define MUSE_GYRO_SCALE_FACTOR 0.0074768

static BLEUUID    serviceUUID("0000fe8d-0000-1000-8000-00805f9b34fb");           //Service UUID
static BLEUUID    charControlUUID("273e0001-4c4d-454d-96be-f03bac821358");       //Characteristic UUID of Control
static BLEUUID    charGyroUUID("273e0009-4c4d-454d-96be-f03bac821358");          //Characteristic UUID of Gyro
static BLEUUID    charAccUUID("273e000a-4c4d-454d-96be-f03bac821358");           //Characteristic UUID of Accelerometer
static BLEUUID    charBaterryUUID("273e000b-4c4d-454d-96be-f03bac821358");       //Characteristic UUID of Battery

static BLERemoteCharacteristic* pRemoteCharacteristicAcc;
static BLERemoteCharacteristic* pRemoteCharacteristicGyro;
static BLERemoteCharacteristic *pRemoteCharacteristicControlChannel;

String My_BLE_Address = "00:55:da:b5:4f:0b";
BLEClient *client;
BLERemoteService* pRemoteService;
unsigned long connectionTime = 0;

String startDataFlowCmd = "\x02\x64\x0a";

int32_t imu[3];

bool ctrlAssigned = false;


void gattcEventHandler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    if (event == ESP_GATTC_DISCONNECT_EVT)
        Serial.printf("gattc event %i detected, disconnect reason: %i\n", (uint8_t)event, (int)param->disconnect.reason);
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient *pclient) {
        Serial.println("Connected to Muse Headset");
        connectionTime = millis();
    }

    void onDisconnect(BLEClient *pclient) {
        Serial.printf("Device disconnected after %ld ms\n", millis() - connectionTime);
    }
};

int16_t decode16bitInt(uint8_t vH, uint8_t vL) {
  int16_t value = (vH << 8) | (vL & 0xff);
  return value;
}

void decodeIMU(uint8_t *data, float scale) {
  for (int i = 0; i < 3; i++) imu[i] = 0;
  for(int i = 2, j = 0; i < 20; i += 2){
    imu[j] += decode16bitInt(data[i], data[i+1]);
    j++;
    j = j%3;
  }
  for (int i = 0; i < 3; i++) imu[i] = (imu[i]/3)* scale;
}



void IMUDataHandler(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  float scale;
  bool permission = false;
  if(pBLERemoteCharacteristic == pRemoteCharacteristicAcc) {
    Serial.print("Acc: ");
    scale = MUSE_ACCELEROMETER_SCALE_FACTOR;
    permission = true;
  }
  else if(pBLERemoteCharacteristic == pRemoteCharacteristicGyro) {
    Serial.print("Gyro: ");
    scale = MUSE_GYRO_SCALE_FACTOR;
    permission = true;
  }
  if(permission){
    for (int i = 0; i < length; i++) {
      Serial.print(pData[i], HEX);
      Serial.print(" ");
    }
    decodeIMU(pData, scale);
    Serial.println("\n----------");
  }
}

bool connectMuse() {
  BLEDevice::init("");
  BLEDevice::setCustomGattcHandler(gattcEventHandler);
  client = BLEDevice::createClient();
  client->setClientCallbacks(new MyClientCallback());
  return client->connect(BLEAddress(My_BLE_Address.c_str()));
}

bool getMainService() {
  pRemoteService = client->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to get service");
    return false;
  }
  else {
    Serial.println("Got service!");
    return true;
  }
}

bool getAccelCharacteristic(notify_callback func){
  pRemoteCharacteristicAcc = pRemoteService->getCharacteristic(charAccUUID);
  if (pRemoteCharacteristicAcc == nullptr) {
    Serial.println("Failed to get characteristic");
    return false;
  }
  else {
    Serial.println("Assigned to Acc characteristic");
    pRemoteCharacteristicAcc->registerForNotify(func);
    return true;
  }
}

bool getGyroCharacteristic(notify_callback func){
  pRemoteCharacteristicGyro = pRemoteService->getCharacteristic(charGyroUUID);
  if (pRemoteCharacteristicGyro == nullptr) {
    Serial.println("Failed to get characteristic");
    return false;
  }
  else {
    Serial.println("Assigned to Gyro characteristic");
    pRemoteCharacteristicGyro->registerForNotify(func);
    return true;
  }
}

bool startCtrlCharacteristic(){
    pRemoteCharacteristicControlChannel = pRemoteService->getCharacteristic(charControlUUID);
    if (pRemoteCharacteristicControlChannel != nullptr) {
        Serial.println("Connected to control channel characteristic, sending command to start streaming data");
        ctrlAssigned = true;
        return true;
    }
    else {
      Serial.println("Required remote characteristic not found");
      return false;
    }
}



void startStream() {
  if(!ctrlAssigned) startCtrlCharacteristic();
  pRemoteCharacteristicControlChannel->writeValue((unsigned char *)startDataFlowCmd.c_str(), startDataFlowCmd.length(), false);
}


void setup() {
    Serial.begin(115200); //Start serial monitor
    Serial.println("ESP32 BLE Server program"); //Intro message

    if(connectMuse()) {
      if(getMainService()){
        if(getGyroCharacteristic(IMUDataHandler) && getAccelCharacteristic(IMUDataHandler)) {
          startStream();
        }
      }
    }
}

void loop() {

}
