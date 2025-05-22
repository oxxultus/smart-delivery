#include "WiFiConnector.h"

WiFiConnector::WiFiConnector(const char* ssid, const char* password)
    : ssid(ssid), password(password) {}

void WiFiConnector::connect() {
    WiFi.begin(ssid, password);
    Serial.print("[WiFiConnector] 연결 시도 중");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFiConnector] 연결 성공! IP: " + WiFi.localIP().toString());
}

bool WiFiConnector::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}