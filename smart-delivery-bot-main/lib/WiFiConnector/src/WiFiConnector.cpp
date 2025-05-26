#include "WiFiConnector.h"

WiFiConnector::WiFiConnector(const char* ssid, const char* password)
    : ssid(ssid), password(password) {}

void WiFiConnector::connect() {
    WiFi.begin(ssid, password);
    Serial.println("[WiFiConnector][1/2] 연결 시도 중");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println("[WiFiConnector][2/2] 연결 성공! IP: " + WiFi.localIP().toString());Serial.println("");
}

bool WiFiConnector::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}