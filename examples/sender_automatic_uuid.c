/*
* This example will spin a BLE iBeacon with automatically generated random UUID
* assigned to the Beacon.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ibeacon.h"

void ble_sender_function()
{

    esp_ibeacon_mode_set(SENDER);
    esp_automatic_random_uuid();
    init_ble_ibeacon();
}

void app_main()
{
    ble_sender_function();
}
