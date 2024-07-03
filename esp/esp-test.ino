#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // HTTP GET request
    http.begin("http://httpbin.org/get");  // Specify the URL
    int httpCode = http.GET();  // Make the request

    if (httpCode > 0) { // Check for the returning code
      String payload = http.getString();  // Get the request response payload
      Serial.println("GET Response Code: " + String(httpCode));
      Serial.println("Response: " + payload);
    } else {
      Serial.println("GET request failed");
    }
    http.end();  // Free resources

    // HTTP POST request
    http.begin("http://httpbin.org/post");  // Specify the URL
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Specify content-type header
    int httpCodePost = http.POST("param1=value1&param2=value2");  // Send the request

    if (httpCodePost > 0) { // Check for the returning code
      String payloadPost = http.getString();  // Get the request response payload
      Serial.println("POST Response Code: " + String(httpCodePost));
      Serial.println("Response: " + payloadPost);
    } else {
      Serial.println("POST request failed");
    }
    http.end();  // Free resources
  } else {
    Serial.println("Error in WiFi connection");
  }

  delay(10000);  // Send a request every 10 seconds
}