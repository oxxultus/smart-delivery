#include <Arduino.h>
#include <WiFiModule.h>
#include "Config.h"
/*
// WiFi 모듈 인스턴스
WiFiModule wifi;

void setup() {
    Serial.begin(9600);
    delay(2000);  // ESP 안정화 대기

    wifi.begin(WIFI_SSID, WIFI_PASSWORD); // 연결 될 때 까지 10번 반복하는 로직이 포함
}

void loop() {
    // 서버에서 명령 받아오기 (GET)
    String command = wifi.getCommand("/sensor");
    Serial.print("서버 명령(GET): ");
    Serial.println(command);

    // 서버에 상태 데이터 전송 (POST)
    String json = R"({"status":"ok","temp":23.5})";  // String json = "{\"status\":\"ok\",\"temp\":23.5}";
    String response = wifi.postData(json, "/sensor");
    Serial.print("서버 응답(POST): ");
    Serial.println(response);

    delay(10000);  // 10초 간격으로 GET/POST 실행
}*/


#include <SoftwareSerial.h>

// 소프트웨어 시리얼: TX=3, RX=2 (RX만 사용)
SoftwareSerial softSerial(2, 3);  // RX, TX

void setup() {
    Serial.begin(9600);             // 수신된 데이터 모니터 출력용
    softSerial.begin(9600);         // 통신용

    Serial.println("Receiver 시작됨");
}

void loop() {
    if (softSerial.available()) {
        char c = softSerial.read();   // 받은 문자 읽기
        Serial.print("수신됨: ");
        Serial.println(c);
    }
}