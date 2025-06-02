#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "DHT.h"
#include <Wire.h>
#include <DFRobot_SHT20.h>
#include <RTClib.h>

// --- Sensor Definitions ---
#define DHT_PIN 47
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
DFRobot_SHT20 sht20;
RTC_DS3231 rtc;

// --- I2C Pins for RTC and SHT20 ---
#define I2C_SDA_PIN 41
#define I2C_SCL_PIN 42

// --- Hall Sensor Pins ---
#define HALL_SENSOR1 3
#define HALL_SENSOR2 2
volatile int rainCount = 0;
bool magneticFieldDetected1 = false;
bool magneticFieldDetected2 = false;

// --- Timing Control ---
unsigned long lastTransmissionTime = 0;
const unsigned long transmissionInterval = 2000; // in milliseconds

// --- LoRa Settings ---
#define RF_FREQUENCY 915000000 // Hz
#define TX_OUTPUT_POWER 5      // dBm
#define LORA_BANDWIDTH 0
#define LORA_SPREADING_FACTOR 7
#define LORA_CODINGRATE 1
#define LORA_PREAMBLE_LENGTH 8
#define LORA_SYMBOL_TIMEOUT 0
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE 200
char txPacket[BUFFER_SIZE];
bool loraIdle = true;

static RadioEvents_t RadioEvents;
void OnTxDone(void);
void OnTxTimeout(void);

// --- Setup ---
void setup() {
  Serial.begin(115200);

  // Initialize I2C and sensors
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  sht20.initSHT20(); delay(100);
  sht20.checkSHT20();
  dht.begin();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Failed to initialize RTC.");
    while (1);
  }

  // Initialize hall sensors
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);
  magneticFieldDetected1 = (digitalRead(HALL_SENSOR1) == HIGH);
  magneticFieldDetected2 = (digitalRead(HALL_SENSOR2) == HIGH);
  rainCount = 0;

  // Initialize LoRa
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

  Serial.println("System initialized with RTC.");
}

// --- Main Loop ---
void loop() {
  Radio.IrqProcess();
  checkRainSensors();

  unsigned long currentTime = millis();
  if (loraIdle && (currentTime - lastTransmissionTime >= transmissionInterval)) {
    lastTransmissionTime = currentTime;

    float dhtTemperature = dht.readTemperature();
    float dhtHumidity = dht.readHumidity();
    float shtTemperature = sht20.readTemperature();
    float shtHumidity = sht20.readHumidity();
    float rainMM = rainCount * 0.2794;
    DateTime now = rtc.now();

    if (isnan(dhtTemperature) || isnan(dhtHumidity) || isnan(shtTemperature) || isnan(shtHumidity)) {
      Serial.println("Sensor read failed.");
      return;
    }

    char timestamp[30];
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
             now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

    snprintf(txPacket, BUFFER_SIZE,
      "{\"dht_temperature\":%.1f,\"dht_humidity\":%.1f,\"sht_temperature\":%.1f,\"sht_humidity\":%.1f,\"rain\":%.2f,\"local_date_read\":\"%s\",\"device_id\":1}",
      dhtTemperature, dhtHumidity, shtTemperature, shtHumidity, rainMM, timestamp);

    Serial.printf("\n%s", txPacket);
    Radio.Send((uint8_t *)txPacket, strlen(txPacket));
    loraIdle = false;
  }
}

// --- LoRa Callbacks ---
void OnTxDone(void) {
  //Serial.println("Transmission completed.");
  loraIdle = true;
}

void OnTxTimeout(void) {
  Radio.Sleep();
  Serial.println("Transmission timeout.");
  loraIdle = true;
}

// --- Rain Sensor Logic (Hall Effect) ---
void checkRainSensors() {
  int state1 = digitalRead(HALL_SENSOR1);
  int state2 = digitalRead(HALL_SENSOR2);

  if (state1 == HIGH && !magneticFieldDetected1) {
    rainCount++;
    magneticFieldDetected1 = true;
  } else if (state1 == LOW) {
    magneticFieldDetected1 = false;
  }

  if (state2 == HIGH && !magneticFieldDetected2) {
    rainCount++;
    magneticFieldDetected2 = true;
  } else if (state2 == LOW) {
    magneticFieldDetected2 = false;
  }
}
