#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- Configurações Wi-Fi ---
constexpr const char* WIFI_SSID     = "SEU_WIFI";
constexpr const char* WIFI_PASS     = "SENHA_WIFI";

// --- Configurações MQTT ---
constexpr const char* MQTT_SERVER   = "192.168.1.100";
constexpr int         MQTT_PORT     = 1883;
constexpr const char* MQTT_TOPIC    = "sensor/temperatura";

// --- Parâmetros do termistor ---
constexpr int   ANALOG_PIN           = A0;
constexpr float SERIES_RESISTOR      = 10000.0;
constexpr float NOMINAL_RESISTANCE   = 10000.0;
constexpr float NOMINAL_TEMPERATURE  = 25.0;
constexpr float B_COEFFICIENT        = 3950.0;
constexpr float ADC_MAX              = 1023.0;
constexpr float VCC                  = 3.3;

// --- Controle de tempo ---
constexpr unsigned long SEND_INTERVAL = 5000;
unsigned long lastSendTime = 0;

// --- Cliente WiFi e MQTT ---
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void connectToWiFi() {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado com IP: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

float readTemperatureCelsius() {
  int analogValue = analogRead(ANALOG_PIN);

  // Evita leitura inválida
  if (analogValue == 0) return -273.15;

  float voltage = analogValue * VCC / ADC_MAX;
  float resistance = SERIES_RESISTOR * (VCC / voltage - 1.0);

  float steinhart = resistance / NOMINAL_RESISTANCE;
  steinhart = log(steinhart);
  steinhart /= B_COEFFICIENT;
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  return steinhart;
}

void publishTemperature() {
  float temperature = readTemperatureCelsius();

  // Cria JSON usando ArduinoJson
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["temperatura"] = temperature;

  char payload[128];
  serializeJson(jsonDoc, payload);

  Serial.println("Publicando JSON via MQTT:");
  Serial.println(payload);

  mqttClient.publish(MQTT_TOPIC, payload);
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = now;
    publishTemperature();
  }
}