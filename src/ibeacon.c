/*
 * This component is based on bluedroid stack of ESP-IDF.
 * The main feature of this component is to enable the user
 * to quickly spin up the ibeacons receivers and transmitters.
 *
 * iBeacon is a trademark of Apple Inc.
 * Before building devices which use iBeacon technology,
 * visit https://developer.apple.com/ibeacon/ to obtain a license.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_ibeacon_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "ibeacon.h"
#include "uuid.h"

static const char *TAG = "IBEACON";
bool mode_bool = 0; // 0 for receiver, 1 for sender

void esp_ibeacon_mode_set(ibeacon_mode_t mode_set)
{

    if (mode_set == RECEIVER)
    {
        mode_bool = 0;
    }
    else
        mode_bool = 1;
}

/* Vendor part of iBeacon data*/
esp_ble_ibeacon_vendor_t vendor_config_2 = {
    .proximity_uuid = {0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .major = ENDIAN_CHANGE_U16(ESP_MAJOR), // Major=ESP_MAJOR
    .minor = ENDIAN_CHANGE_U16(ESP_MINOR), // Minor=ESP_MINOR
    .measured_power = 0xC5};
void esp_custom_uuid(uint8_t user_uuid[])
{
    memcpy(vendor_config_2.proximity_uuid, user_uuid, 16);
}

void esp_automatic_random_uuid()
{
    uuid_t uu;
    uuid_generate(uu);
    char uu_str[UUID_STR_LEN];
    uuid_unparse(uu, uu_str);
    ESP_LOGI(TAG, "UUID generated and assigned is : %s", uu_str);
    memcpy(vendor_config_2.proximity_uuid, uu, 16);
}

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval = 0x50,
    .scan_window = 0x30,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE};

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_NONCONN_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void esp_custom_setting_sender_parameters(esp_ble_adv_params_t user_defined_params)
{
    ble_adv_params = user_defined_params;
}

void esp_custom_setting_receiver_parameters(esp_ble_scan_params_t user_defined_params)
{
    ble_scan_params = user_defined_params;
}

void esp_ibeacon_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
    {
        if (mode_bool == 1)
            esp_ble_gap_start_advertising(&ble_adv_params);
        else
            break;
    }
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
    {
        if (mode_bool == 0)
        {
            // the unit of the duration is second, 0 means scan permanently
            uint32_t duration = 0;
            esp_ble_gap_start_scanning(duration);
        }
        else
            break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        // scan start complete event to indicate scan start successfully or failed
        if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Scan start failed: %s", esp_err_to_name(err));
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        // adv start complete event to indicate adv start successfully or failed
        if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Adv start failed: %s", esp_err_to_name(err));
        }
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
    {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt)
        {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            /* Search for BLE iBeacon Packet */
            if (esp_ble_is_ibeacon_packet(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len))
            {
                esp_ble_ibeacon_t *ibeacon_data = (esp_ble_ibeacon_t *)(scan_result->scan_rst.ble_adv);
                ESP_LOGI(TAG, "----------iBeacon Found----------");
                esp_log_buffer_hex("IBEACON: Device address:", scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
                esp_log_buffer_hex("IBEACON: Proximity UUID:", ibeacon_data->ibeacon_vendor.proximity_uuid, ESP_UUID_LEN_128);

                uint16_t major = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.major);
                uint16_t minor = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.minor);
                ESP_LOGI(TAG, "Major: 0x%04x (%d)", major, major);
                ESP_LOGI(TAG, "Minor: 0x%04x (%d)", minor, minor);
                ESP_LOGI(TAG, "Measured power (RSSI at a 1m distance):%d dbm", ibeacon_data->ibeacon_vendor.measured_power);
                ESP_LOGI(TAG, "RSSI of packet:%d dbm", scan_result->scan_rst.rssi);
            }
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Scan stop failed: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(TAG, "Stop scan successfully");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Adv stop failed: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(TAG, "Stop adv successfully");
        }
        break;
    default:
        break;
    }
}

void ble_ibeacon_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(TAG, "register callback");

    // register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_ibeacon_handler)) != ESP_OK)
    {
        ESP_LOGE(TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }
}

void init_ble_ibeacon(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();
    ble_ibeacon_appRegister();

    /* set scan parameters */
    if (mode_bool == 0)
    {
        esp_ble_gap_set_scan_params(&ble_scan_params);
        ESP_LOGI(TAG, "***Receiver Mode***");
    }
    else
    {
        ESP_LOGI(TAG, "***Sender Mode***");
        esp_ble_ibeacon_t ibeacon_adv_data;
        esp_err_t status = esp_ble_config_ibeacon_data(&vendor_config_2, &ibeacon_adv_data);
        if (status == ESP_OK)
        {
            esp_ble_gap_config_adv_data_raw((uint8_t *)&ibeacon_adv_data, sizeof(ibeacon_adv_data));
        }
        else
        {
            ESP_LOGE(TAG, "Config iBeacon data failed: %s\n", esp_err_to_name(status));
        }
    }
}
