#include <Arduino.h>
#include "BLEDevice.h"
#include "museConstants.h"
#include "muse32.h"


void gattcEventHandler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    if (event == ESP_GATTC_DISCONNECT_EVT) {
        Serial.printf("gattc event %i detected, disconnect reason: %i\n", (uint8_t)event, (int)param->disconnect.reason);
    }
}

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
