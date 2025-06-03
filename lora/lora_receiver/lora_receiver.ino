#include "LoRaWan_APP.h"
#include "Arduino.h"

// --- LoRa Configuration ---
#define RF_FREQUENCY 915000000 // Hz
#define LORA_BANDWIDTH 0
#define LORA_SPREADING_FACTOR 7
#define LORA_CODINGRATE 1
#define LORA_PREAMBLE_LENGTH 8
#define LORA_SYMBOL_TIMEOUT 0
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE 256 // Increased buffer size to handle larger packets

// --- Receive Buffer ---
char rxpacket[BUFFER_SIZE];
static RadioEvents_t RadioEvents;

int16_t rssi = 0;
uint16_t rxSize = 0;
bool lora_idle = true;

// --- Callback function on successful packet reception ---
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi_, int8_t snr);

void setup() {
  Serial.begin(115200);
  delay(100);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  // Initialize LoRa with callback
  RadioEvents.RxDone = OnRxDone;
  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
}

void loop() {
  if (lora_idle) {
    lora_idle = false;
    // Serial.println("Waiting for incoming packet...");
    Radio.Rx(0); // Continuous receive mode
  }

  Radio.IrqProcess(); // Process interrupt events
}

// --- Handle received packet ---
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi_, int8_t snr) {
  rxSize = size;
  rssi = rssi_;

  // Prevent buffer overflow when copying payload
  if (size >= BUFFER_SIZE) size = BUFFER_SIZE - 1;

  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';

  Radio.Sleep();
  Serial.println(rxpacket);

  // Print RSSI and packet size if necessary
  // Serial.printf("RSSI: %d | Size: %d bytes\n", rssi, rxSize);

  lora_idle = true;
}
