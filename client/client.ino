#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

const char * SSID = "${WIFI_SSID}";
const char * WIFI_PASSWORD = "${WIFI_PASSWORD}";

const String SERVER_URL = "${SERVER_URL}";
const String SENSOR_NAME = "${SENSOR_NAME}";
const int SERVER_PORT = ${SERVER_PORT};

const int MEASURE_POWER = 5; // D1
const int MEASURE_IN = 0; // A0

const int SAMPLE_COUNT = 8;
const double SAMPLE_WEIGHTS[] = {16, 16, 8, 2 , 2, 8, 16, 16};

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
    delay(10);

    double sum = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        sum += analogRead(MEASURE_IN) / SAMPLE_WEIGHTS[i];
    }
    int val = (int) sum;
    Serial.println(val);

    Serial.println("Disabling power to sensor");
    digitalWrite(MEASURE_POWER, LOW);

    return val;
}

void sendToServer(double sensorVal) {
    Serial.println("Sending value to server");
    
    HTTPClient http;
    Serial.println("Http begin");
    
    http.begin(SERVER_URL, SERVER_PORT, "/api/measurements");
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
