#include <Servo.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

Servo SG90;
WiFiClient client;
HTTPClient http;

const char* ssid = "SUPERONLINE_Wi-Fi_5023";
const char* password = "sbCzCuC7AGXA";

String server_login_url = "http://ctf2023-demo.inderse.com/sandbox/auth/login";
String payload = "{\"test_key\": \"LLp4ypnykxqvhU6CCLNUs2gns2bhYNMhacK3wwLGTNHh3yEc\"}";
String server_get_position_url = "http://ctf2023-demo.inderse.com/sandbox/state";

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");
  SG90.attach(4);
  delay(500);
  SG90.write(0);
  delay(500);
}

void loop() {
  if (Serial.available()) {
    int callApi = Serial.read();
    if (callApi == 97) {
      String response_api = login_with_test_key();
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, response_api);
      const char* split_access_token = doc["access_token"];
      get_motor_position(split_access_token);
    }
  }
}

String login_with_test_key() {
  http.begin(client, server_login_url);
  http.addHeader("Content-Type", "application/json");
  int http_response_code = http.POST(payload);
  Serial.print("\nHTTP Status : ");
  Serial.print(http_response_code);
  String all_access_token = http.getString();
  return all_access_token;
}

void get_motor_position(String authorization_token) {
  http.begin(client, server_get_position_url);
  http.addHeader("Authorization", authorization_token);
  int http_response_code = http.GET();
  if (http_response_code == 200) {
    String motor_position = http.getString();
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, motor_position);
    int value = doc["value"]; 
    Serial.print("\nAngle value : ");
    Serial.print(value);
    SG90.write(value);
    delay(500);
    http.end();
  }
  else{
    Serial.println("\nHTTP Status Error");
    Serial.println(http_response_code);
  }
}
