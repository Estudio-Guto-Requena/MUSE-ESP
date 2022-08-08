#ifndef MUSE_CONSTANTS
#define MUSE_CONSTANTS

#include <Arduino.h>
#include "BLEDevice.h"

static BLEUUID    serviceUUID("0000fe8d-0000-1000-8000-00805f9b34fb");           //Service UUID
static BLEUUID    charControlUUID("273e0001-4c4d-454d-96be-f03bac821358");       //Characteristic UUID of Control
static BLEUUID    charGyroUUID("273e0009-4c4d-454d-96be-f03bac821358");          //Characteristic UUID of Gyro
static BLEUUID    charAccUUID("273e000a-4c4d-454d-96be-f03bac821358");           //Characteristic UUID of Accelerometer
static BLEUUID    charBaterryUUID("273e000b-4c4d-454d-96be-f03bac821358");       //Characteristic UUID of Battery

const String startDataFlowCmd = "\x02\x64\x0a";

#endif
