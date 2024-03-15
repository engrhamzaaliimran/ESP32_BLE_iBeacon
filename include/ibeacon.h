
#ifndef _BLE_IBEACON_H_
#define _BLE_IBEACON_H_

#include "esp_gap_ble_api.h"
#include <stdbool.h>

/*
 * Current ESP IDF version doesn't support scan and adv simultaneously,
 * so ibeacon sender and receiver should not run simultaneously.
 * This enum is used for controlling in which mode we want our device to work.
 */
typedef enum
{
    SENDER,   /* For enabling Sender mode */
    RECEIVER, /* For enabling Receiver mode*/
} ibeacon_mode_t;

/**
 * @brief Set the ibeacon working mode
 *
 * @param mode_set  if set SENDER enables the device to work as the sender, else for RECEIVER device works as a receiver
 *
 * This function sets the internal parameters of the code to work as Sender or Receiver
 * depending upon the enum parameter passed to the function. If this function is not invoked SENDER mode is set by default,
 */

void esp_ibeacon_mode_set(ibeacon_mode_t mode_set);

/**
 * @brief Use to update default sender beacons parameters.
 * This function can be used to update all the advanced parameters of the sender mode of the program.
 *
 * @param user_defined_params The structure contains details related to the sender mode of ibeacon.
 *
 * Following are the details
 *   - adv_int_min Minimum advertising interval for undirected and low duty cycle directed advertising.
 *                Range: 0x0020 to 0x4000. The default value set is 0x20
 *   - adv_int_max Maximum advertising interval for undirected and low duty cycle directed advertising.
 *                Range: 0x0020 to 0x4000. The default value set is 0x40
 *   - adv_type Advertising type
 *   - own_addr_type Owner bluetooth device address type
 *   - channel_map Advertising channel map
 *   - adv_filter_policy Advertising filter policy
 */
void esp_custom_setting_sender_parameters(esp_ble_adv_params_t user_defined_params);

/**
 * @brief Use to update default receiver beacons parameters.
 * 
 * This function can be used to update all the parameters of the receiver mode of the program.
 *
 * @param user_defined_params The structure contains details related to the receiver mode of ibeacon.
 *
 * Following are the details
 *   - scan_type Scan type
 *               Can be BLE_SCAN_TYPE_PASSIVE or BLE_SCAN_TYPE_ACTIVE
 *   - own_addr_type Scan filter policy
 *   - scan_filter_policy Scan filter policy
 *   - scan_interval Scan interval. This is defined as the time interval from
 *                when the Controller started its last LE scan until it begins the subsequent LE scan.
 *                Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
 *   - scan_window Scan window. The duration of the LE scan. LE_Scan_Window
 *                shall be less than or equal to LE_Scan_Interval
 *                Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
 *                Time = N * 0.625 msec
 *                Time Range: 2.5 msec to 10240 msec
 *   - scan_duplicate The Scan_Duplicates parameter controls whether the Link Layer should filter out
 *                duplicate advertising reports (BLE_SCAN_DUPLICATE_ENABLE) to the Host, or if the Link Layer should generate
 *                advertising reports for each packet received
 */
void esp_custom_setting_receiver_parameters(esp_ble_scan_params_t user_defined_params);

/**
 * @brief Handles all the cases for both receiver and sender mode of ibeacon.
 *
 * This function handles all scenarios of ibeacon working either as sender or receiver.
 * The user don't need to invoke it in his/her code.
 */
void esp_ibeacon_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/**
 * @brief The internal function used by the handler
 *
 * This function registers the scan callback function to the gap module.
 */
void ble_ibeacon_appRegister(void);

/**
 * @brief This function is use to set the UUID value.
 *
 * @param user_uuid[] is a 16 bit array that will contain custom UUID
 * 
 * This function is use to set the UUID value. Given by the user. By default all 16 bit are set to zero.
 */
void esp_custom_uuid(uint8_t user_uuid[]);

/**
 * @brief This function is use to set the automatically generated random UUID value.
 * 
 * This function is use to set the automatically generated random UUID value. It generated 
 */
void esp_automatic_random_uuid();

/**
 * @brief  Dose initial settings and starts the ibeacon process according to the mode selected
 *
 * This function does the nvs settings, initiate the bluedroid stack, enables the BLE mode of
 * bluedroid and starts the ibeacon processing according to the mode selected by the user.
 */
void init_ble_ibeacon();

#endif
