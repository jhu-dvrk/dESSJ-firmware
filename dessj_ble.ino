/*
  Battery Monitor

  This example creates a Bluetooth® Low Energy peripheral with the standard battery service and
  level characteristic. The A0 pin is used to calculate the battery level.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

 // Bluetooth® Low Energy Battery Service
BLEService sujservice("babb122c-de4c-11ec-9d64-0242ac120100");
BLEService potservice("babb122c-de4c-11ec-9d64-0242ac120000");

uint16_t all_pots_values[8] = {0};

BLEStringCharacteristic all_pots_char("babb122c-de4c-11ec-9d64-0242ac120101",  // standard 16-bit characteristic UUID
    BLERead | BLENotify, 128); // remote clients will be able to get notifications if this characteristic changes

uint8_t analog_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
char output_string[128] = "";

void setup() {
  Serial.begin(9600);    // initialize serial communication
  analogReadResolution(12);

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
  for (int i = 0; i < 8; i++) {
    pinMode(analog_pins[i], INPUT); // initialize the built-in LED pin to indicate when a central is connected
  }

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the Bluetooth® Low Energy device
     This name will appear in advertising packets
     and can be used by remote devices to identify this Bluetooth® Low Energy device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setDeviceName("dVRK SUJ");
  BLE.setLocalName("dVRK SUJ");
  
  sujservice.addCharacteristic(all_pots_char);
  BLE.addService(sujservice);
  BLE.setAdvertisedService(sujservice); 

  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLE.poll();
  for (int i = 0; i < 8; i++) {
    uint32_t sum = 0;
    for (int j = 0; j < 16 * 16; j++) {
      sum += analogRead(analog_pins[i]);      
    }
    uint16_t averaged = sum / 16;
    all_pots_values[i] = averaged;
  }
  sprintf(output_string, "{\"timestamp\":%U, \"pots\":[%u,%u,%u,%u,%u,%u,%u,%u]}", millis(), all_pots_values[0], all_pots_values[1], all_pots_values[2], all_pots_values[3], all_pots_values[4], all_pots_values[5], all_pots_values[6], all_pots_values[7]);
  all_pots_char.writeValue(output_string);
}
