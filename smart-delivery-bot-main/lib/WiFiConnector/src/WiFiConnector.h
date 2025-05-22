#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <ESP8266WiFi.h>

class WiFiConnector {
private:
    const char* ssid;
    const char* password;

public:
    WiFiConnector(const char* ssid, const char* password);
    void connect();  // 연결 시도 + 상태 출력
    bool isConnected() const;
};

#endif