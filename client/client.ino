#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

const char * SSID = "ssid";
const char * WIFI_PASSWORD = "password";

const String SERVER_URL = "https://192.168.1.3";
const String SENSOR_NAME = "Sensor 1";

ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup start");
    
    prepareWifi();
    
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
}

double getSoilMeasurement() {
    // TODO: Read actual value from sensor
    Serial.println("Reading soil value");
    
    return 42.17;
}

void sendToServer(double sensorVal) {
    Serial.println("Sending value to server");
    
    HTTPClient http;
    Serial.println("Http begin");
    
    http.begin(SERVER_URL + "/api/measurements");
    http.addHeader("Content-Type", "application/json");
    http.POST(formatPayload(sensorVal));
    http.end();
}

String formatPayload(double sensorVal) {
    return "{" + 
    json_str_attr("source", SENSOR_NAME) +
    json_num_attr("timestampMillis", 1337) + 
    json_str_attr("type", "soil_moisture") + 
    json_num_attr("value", sensorVal) + 
    json_str_attr("unit", "unit") + 
    "}";
}

String json_str_attr(String name, String value) {
    return "\"" + name + "\": " + "\"" + value + "\" ";
}

String json_num_attr(String name, double value) {
    return "\"" + name + "\": " + "\"" + value + "\" ";
}