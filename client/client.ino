#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

const char * SENSOR_NAME = "Sensor 1";

const char * SSID = "ssid";
const char * WIFI_PASSWORD = "password"

const char * SERVER_URL = "https://192.168.1.3";

void setup() {
    Serial.begin(115200);
    Serial.println("Setup start")
    
    prepareWifi()
    
    Serial.println("Setup end");
}

void prepareWifi() {
    WifiMulti.addAP(SSID, WIFI_PASSWORD);
}

void loop() {
    if (WifiMulti.run() == WL_CONNECTED) {
        double soilVal = getSoilMeasurement();
        
        sendToServer(soilVal)
    }
}

double getSoilMeasurement() {
    // TODO: Read actual value from sensor
    Serial.println("Reading soil value");
    
    return 42.17;
}

void sendToServer(sensorVal) {
    Serial.println("Sending value to server");
    
    HTTPClient http;
    Serial.println("Http begin");
    
    http.begin(SERVER_URL + "/api/measurements");
    http.addHeader("Content-Type", "application/json");
    http.post(formatPayload(sensorVal));
    http.end();
}

const char * formatPayload(sensorVal) {
    return "{" + 
    json_str_attr("source", SENSOR_NAME) +
    json_num_attr("timestampMillis", "1337") + 
    json_str_attr("type", "soil_moisture") + 
    json_num_attr("value", sensorVal) + 
    json_str_attr("unit", "unit") + 
    "}";
}

const char * json_str_attr(name, value) {
    return "\"" + name + "\": " + "\"" + value + "\" ";
}

const char * json_num_attr(name, value) {
    return "\"" + name + "\": " + "\"" + value + "\" ";
}