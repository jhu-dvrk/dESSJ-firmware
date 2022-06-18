#include <ArduinoBLE.h>

BLEService sujservice("babb122c-de4c-11ec-9d64-0242ac120100");

uint16_t all_pots_values[8] = {0};

BLEStringCharacteristic all_pots_char("babb122c-de4c-11ec-9d64-0242ac120101",  // standard 16-bit characteristic UUID
    BLERead | BLENotify, 128); // remote clients will be able to get notifications if this characteristic changes

uint8_t analog_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
char output_string[128] = "";

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);

  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 8; i++) {
    pinMode(analog_pins[i], INPUT);
  }

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

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
