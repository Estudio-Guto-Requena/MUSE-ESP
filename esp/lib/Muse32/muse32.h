/*
Library for Muse 2 EEG sensor
by Bruno Arico.
*/

#ifndef MUSE32_h
#define MUSE32_h

#include <Arduino.h>
#include "museConstants.h"

class Muse {
	private:
		String pairMACAddr;
		bool paired = false;
		bool found = false;
		int32_t imuAcc[3];
		int32_t imuGyro[3];
		static BLERemoteCharacteristic* pRemoteCharacteristic;

		int16_t decode16bitInt(uint8_t vH, uint8_t vL);
		void IMUdataHandler(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
		void decodeIMU(uint8_t *data, float scale);

	public:
		Muse(String macAddr);

		void gattcEventHandler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


};

#endif
