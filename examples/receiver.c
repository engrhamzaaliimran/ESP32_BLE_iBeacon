#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ibeacon.h"


void ble_receiver_function()
{
    esp_ibeacon_mode_set(RECEIVER);
    init_ble_ibeacon();
}

void app_main()
{
    ble_receiver_function();
}
