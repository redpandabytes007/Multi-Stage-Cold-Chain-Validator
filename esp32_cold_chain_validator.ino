/*
  ESP32 Cold Chain Validator Firmware

  This code reads temperature, humidity, and gas sensor data
  and uploads it to the ThingSpeak cloud platform.
  A Spoilage Risk Score (SRS) computed using cloud-based
  MATLAB analytics is fetched back to trigger alerts.

  Hardware:
  - ESP32
  - DS18B20 Temperature Sensor
  - DHT22 Humidity Sensor
  - MQ-135 Gas Sensor

  Note:
  This firmware was developed for an academic mini-project
  under the course CS61066 - Architecture and Protocols for IoT.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define ONE_WIRE_BUS 5
#define DHTPIN 21
#define DHTTYPE DHT22
#define MQ135_PIN 34
#define LED_PIN 23
#define BUZZER_PIN 22

const char* ssid = "Android15";
const char* pass = "qwertyuioppoiuytrewq";

String writeApiKey = "OP93579NP6JJIYEC";
String readApiKey  = "OXJLZOYKI3TGA083";
String channelID   = "3051991";
const char* server = "api.thingspeak.com";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MQ135_PIN, INPUT);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  ds18b20.begin();
  dht.begin();
}

void loop() {

  ds18b20.requestTemperatures();
  float tempC = ds18b20.getTempCByIndex(0);
  float humidity = dht.readHumidity();
  int mq135_value = analogRead(MQ135_PIN);
  float voc_ppm = map(mq135_value, 0, 4095, 0, 500);

  String productName = "Unknown";
  String stageName = "Not Set";

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    String url1 = "https://api.thingspeak.com/channels/" + channelID + "/fields/5/last?api_key=" + readApiKey;
    http.begin(url1);
    int code1 = http.GET();
    if (code1 == 200) {
      int productID = http.getString().toInt();
      switch (productID) {
        case 1: productName = "Milk"; break;
        case 2: productName = "Meat"; break;
        case 3: productName = "Fruits"; break;
        default: productName = "General Food"; break;
      }
    }
    http.end();

    String url2 = "https://api.thingspeak.com/channels/" + channelID + "/fields/6/last?api_key=" + readApiKey;
    http.begin(url2);
    int code2 = http.GET();
    if (code2 == 200) {
      int stageID = http.getString().toInt();
      switch (stageID) {
        case 1: stageName = "Cold Storage"; break;
        case 2: stageName = "Transport Truck"; break;
        case 3: stageName = "Distribution Hub"; break;
        case 4: stageName = "Retail Outlet"; break;
        default: stageName = "Unknown Stage"; break;
      }
    }
    http.end();
  }

  Serial.println("\n----------- SENSOR READINGS -----------");
  Serial.print("Product Type: "); Serial.println(productName);
  Serial.print("Stage: "); Serial.println(stageName);
  Serial.print("Temperature (°C): "); Serial.println(tempC);
  Serial.print("Humidity (%): "); Serial.println(humidity);
  Serial.print("VOC (ppm): "); Serial.println(voc_ppm);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.thingspeak.com/update?api_key=" + writeApiKey +
                 "&field1=" + String(tempC) +
                 "&field2=" + String(tempC) +
                 "&field3=" + String(humidity) +
                 "&field4=" + String(voc_ppm);
    http.begin(url);
    int code = http.GET();
    if (code == 200) Serial.println("Data uploaded to ThingSpeak.");
    else Serial.print("Error sending data: "), Serial.println(code);
    http.end();
  }

  delay(15000);

  float srs = 0.0;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.thingspeak.com/channels/" + channelID + "/fields/7/last?api_key=" + readApiKey;
    http.begin(url);
    int code = http.GET();
    if (code == 200) {
      srs = http.getString().toFloat();
    }
    http.end();
  }

  Serial.print("Spoilage Risk Score (SRS): ");
  Serial.println(srs);
  const float SRS_THRESHOLD = 30.0;

  if (srs > SRS_THRESHOLD) {
    Serial.println("⚠ High Spoilage Risk! Alerting...");
    while (srs > SRS_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
      delay(150);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(150);

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http2;
        String url2 = "https://api.thingspeak.com/channels/" + channelID + "/fields/7/last?api_key=" + readApiKey;
        http2.begin(url2);
        int code2 = http2.GET();
        if (code2 == 200) srs = http2.getString().toFloat();
        http2.end();
      }
    }
    Serial.println("Conditions back to normal. Alert stopped.");
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("✅ Safe Condition. All parameters within limits.");
  }

  delay(10000);
}
  