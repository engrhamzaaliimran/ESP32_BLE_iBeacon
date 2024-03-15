#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ibeacon.h"


void ble_sender_function()
{
    uint8_t my_uuid[16] = {0x45, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};

    esp_ibeacon_mode_set(SENDER);
    esp_custom_uuid(my_uuid);
    init_ble_ibeacon();
}

void app_main()
{
    ble_sender_function();
}
