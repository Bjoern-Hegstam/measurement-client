#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

const char * SSID = "${WIFI_SSID}";
const char * WIFI_PASSWORD = "${WIFI_PASSWORD}";

const String SERVER_URL = "${SERVER_URL}";
const int SERVER_PORT = ${SERVER_PORT};

const String DEBUG_SERVER_URL = "${DEBUG_SERVER_URL}";
const int DEBUG_SERVER_PORT = ${DEBUG_SERVER_PORT};

const String SENSOR_NAME = "${SENSOR_NAME}";

const int MEASURE_POWER_PIN = 5; // D1
const int MEASURE_PIN = 0; // A0

const int DEBUG_MODE_PIN = 13; // D7

const int SAMPLE_COUNT = 8;
const double SAMPLE_WEIGHTS[] = {16, 16, 8, 2 , 2, 8, 16, 16};

ESP8266WiFiMulti WiFiMulti;

int debugEnabled = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup start");
    
    connectWifi();

    pinMode(MEASURE_POWER_PIN, OUTPUT);
    pinMode(MEASURE_PIN, INPUT);
    pinMode(DEBUG_MODE_PIN, INPUT);

    loadDebugConfig();

    Serial.println("Setup end");
}

void loadDebugConfig() {
    debugEnabled = digitalRead(DEBUG_MODE_PIN);
    Serial.print("Debug enabled: ");
    if (debugEnabled == HIGH) {
        Serial.println("1");
    } else {
        Serial.println("0");
    }
}

void connectWifi() {
    WiFi.begin(SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
}

void loop() {
    double soilVal = getSoilMeasurement();
    sendToServer(soilVal);

    if (debugEnabled) {
        ESP.deepSleep(20e6);
    } else {
        ESP.deepSleep(${DEEP_SLEEP_MS});
    }
}

double getSoilMeasurement() {
    Serial.println("Reading soil value");
    digitalWrite(MEASURE_POWER_PIN, HIGH);
    delay(10);

    double sum = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        sum += analogRead(MEASURE_PIN) / SAMPLE_WEIGHTS[i];
    }
    int val = (int) sum;
    Serial.println(val);

    Serial.println("Disabling power to sensor");
    digitalWrite(MEASURE_POWER_PIN, LOW);

    return val;
}

void sendToServer(double sensorVal) {
    Serial.println("Sending value to server");
    
    HTTPClient http;
    Serial.println("Http begin");
    
    if (debugEnabled) {
        http.begin(DEBUG_SERVER_URL, DEBUG_SERVER_PORT, "/api/measurements");
    } else {
        http.begin(SERVER_URL, SERVER_PORT, "/api/measurements");
    }
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
