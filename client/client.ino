#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

const char * SSID = "ssid";
const char * WIFI_PASSWORD = "password";

const String SERVER_URL = "192.168.1.3";
const String SENSOR_NAME = "Sensor 1";
const int SERVER_PORT = 4567;

const int MEASURE_POWER = 16; // D0
const int MEASURE_IN = 0; // A0

ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup start");
    
    prepareWifi();
    
    pinMode(MEASURE_POWER, OUTPUT);
    pinMode(MEASURE_IN, INPUT);

    Serial.println("Setup end");
}

void prepareWifi() {
    WiFiMulti.addAP(SSID, WIFI_PASSWORD);
}

void loop() {
    if (WiFiMulti.run() == WL_CONNECTED) {
        double soilVal = getSoilMeasurement();
        
        sendToServer(soilVal);
    }

    delay(4000);
}

double getSoilMeasurement() {
    Serial.println("Reading soil value");
    digitalWrite(MEASURE_POWER, HIGH);
    delay(100);

    int val = analogRead(MEASURE_IN);
    Serial.println(val);

    Serial.println("Disabling power to sensor");
    digitalWrite(MEASURE_POWER, LOW);

    return val;
}

void sendToServer(double sensorVal) {
    Serial.println("Sending value to server");
    
    HTTPClient http;
    Serial.println("Http begin");
    
    http.begin(SERVER_URL, SERVER_PORT, "/api/measurement/");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(formatPayload(sensorVal));
    http.end();
    Serial.println(httpCode);
}

String formatPayload(double sensorVal) {
    return "{" + 
    json_str_attr("source", SENSOR_NAME) + ", " +
    json_str_attr("type", "soil_moisture") + ", " +
    json_double_attr("value", sensorVal) + ", " +
    json_str_attr("unit", "unit") + 
    "}";
}

String json_str_attr(String name, String value) {
    return "\"" + name + "\": " + "\"" + value + "\"";
}

String json_double_attr(String name, double value) {
    return "\"" + name + "\": " + "\"" + value + "\"";
}
