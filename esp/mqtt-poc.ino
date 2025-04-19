#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- Configurações Wi-Fi ---
constexpr const char* WIFI_SSID     = "SEU_WIFI";
constexpr const char* WIFI_PASS     = "SENHA_WIFI";

// --- Configurações MQTT ---
constexpr const char* MQTT_SERVER   = "192.168.1.100";
constexpr int         MQTT_PORT     = 1883;
constexpr const char* MQTT_TOPIC_DATA      = "sensor/data";
constexpr const char* MQTT_TOPIC_CONTROL   = "sensor/activation";

// --- Configurações do DEVICE ---
constexpr int         UAIPY_DEVICE_ID     = 1;

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

// --- Pino do LED ---
constexpr int LED_PIN = D1;

// --- Cliente WiFi e MQTT ---
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// --- Funções ---
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
      mqttClient.subscribe(MQTT_TOPIC_CONTROL);  // Inscreve no tópico de controle
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

  StaticJsonDocument<256> jsonDoc;
  jsonDoc["device_id"] = UAIPY_DEVICE_ID;
  JsonObject data = jsonDoc.createNestedObject("data");
  data["temperatura"] = temperature;

  char payload[256];
  serializeJson(jsonDoc, payload);

  Serial.println("Publicando JSON via MQTT:");
  Serial.println(payload);

  mqttClient.publish(MQTT_TOPIC_DATA, payload);
}

// Callback para mensagens recebidas
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico ");
  Serial.println(topic);

  StaticJsonDocument<256> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, payload, length);

  if (error) {
    Serial.print("Erro ao fazer parse do JSON: ");
    Serial.println(error.c_str());
    return;
  }

  int deviceId = jsonDoc["device_id"];
  if (deviceId != UAIPY_DEVICE_ID) return;  // Ignora mensagens de outros dispositivos

  bool irrigation = jsonDoc["data"]["irrigation_pump"];

  if (irrigation) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Irrigation ON - LED ligado");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Irrigation OFF - LED desligado");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Garante que o LED inicie desligado

  connectToWiFi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
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
