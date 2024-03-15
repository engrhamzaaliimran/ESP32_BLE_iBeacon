# Component BLE iBeacon

This component provides the API calls for quickly spinning up the iBeacons based on ESP32 devices. It allows the user to create both the ibeacon receivers and transmitters. The user can control all major parameters of ibeacon by making simple API function calls.  

The current ESP IDF version doesn't support scan and adv simultaneously, so ibeacon sender and receiver should not run simultaneously. This enum is used for controlling in which mode we want our device to work. iBeacon is a trademark of Apple Inc. Before building devices which use iBeacon technology,
 * visit https://developer.apple.com/ibeacon/ to obtain a license.

## API functions details
The details of all API functions are given below

 1. void esp_ibeacon_mode_set(ibeacon_mode_t mode_set)

 This function sets the internal parameters of the code to work as Sender or Receiver depending upon the enum parameter passed to the function. If this function is not invoked SENDER mode is set by default. The parameter *mode_set*  if set **SENDER** enables the device to work as the sender, else for **RECEIVER**, the device works as a receiver
 
 2. void esp_custom_setting_sender_parameters(esp_ble_adv_params_t user_defined_params)

This function can be used to update all the parameters of the receiver mode of the program. The parameter *user_defined_params* is a structure that contains details related to the sender mode of ibeacon. Details can be found in ibeacon.h.

3. void esp_custom_setting_receiver_parameters(esp_ble_scan_params_t user_defined_params)

This function can be used to update default receiver beacon parameters. The parameter *user_defined_params* is a structure that contains details related to the receiver mode of ibeacon. Details can be found in ibeacon.h.

4. void esp_custom_uuid(uint8_t user_uuid[])

This function is used to set the UUID value. The parameter *user_uuid[]* is a 16-bit array that will contain custom UUID.

5. void init_ble_ibeacon()

Dose initial settings and start the ibeacon process according to the mode selected. 
