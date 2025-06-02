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
#define BUFFER_SIZE 64

// --- Receive Buffer ---
char rxBuffer[BUFFER_SIZE];
static RadioEvents_t radioEvents;

int16_t rssi;
int16_t packetSize;
bool loraIdle = true;

// --- Callback function on successful packet reception ---
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi_, int8_t snr);

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  // Initialize LoRa with callback
  radioEvents.RxDone = OnRxDone;
  Radio.Init(&radioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
}

void loop() {
  if (loraIdle) {
    loraIdle = false;
    Serial.println("Waiting for incoming packet...");
    Radio.Rx(0); // Continuous receive mode
  }

  Radio.IrqProcess(); // Process interrupt events
}

// --- Handle received packet ---
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi_, int8_t snr) {
  packetSize = size;
  rssi = rssi_;

  memcpy(rxBuffer, payload, size);
  rxBuffer[size] = '\0';

  Radio.Sleep();
  Serial.println(rxBuffer);

  // Print RSSI and packet size if necessary
  // Serial.printf("RSSI: %d | Size: %d bytes\n", rssi, packetSize);

  loraIdle = true;
}
