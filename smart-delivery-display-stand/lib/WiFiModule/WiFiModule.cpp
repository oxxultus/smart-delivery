//
// Created by 영진 on 25. 5. 13.
//

#include <Arduino.h>
#include "WiFiModule.h"
#include "Config.h"  // 설정 정보 사용

/// -------------------------------------------------------------------------------------------------------------------
/// 생성자 및 초기화 관련
/// -------------------------------------------------------------------------------------------------------------------

// 생성자: SoftwareSerial 객체 초기화 (RX = 2, TX = 3)
WiFiModule::WiFiModule() : esp(2, 3) {}

/// -------------------------------------------------------------------------------------------------------------------
/// public 메서드
/// -------------------------------------------------------------------------------------------------------------------

// WiFi 연결 초기화
void WiFiModule::begin(const String& ssid, const String& password) {
    // WiFi 연결 시도 시작 메시지 출력
    Serial.println("WiFi 연결 시도 중...");

    // ESP-01과의 시리얼 통신 시작 (보레이트 9600bps)
    esp.begin(9600);

    // 기본 AT 명령 전송 → ESP 응답 확인
    sendCommand("AT", 1000);

    // WiFi 모드 설정: Station 모드로 전환
    sendCommand("AT+CWMODE=1", 1000);

    int attempts = 0;                // 현재 시도 횟수
    const int maxAttempts = 10;      // 최대 시도 횟수 제한

    // 최대 10회까지 연결 시도 루프
    while (attempts < maxAttempts) {
        Serial.print("연결 시도 ");
        Serial.print(attempts + 1);
        Serial.println(" / 10");

        // AT 명령으로 SSID/PASSWORD 기반 WiFi 접속 시도
        sendCommand("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"", 5000);

        // ESP 응답 수신
        String response = readResponse();

        // 응답에 "WIFI CONNECTED" 또는 "OK"가 포함되어 있으면 연결 성공
        if (response.indexOf("WIFI CONNECTED") != -1 || response.indexOf("OK") != -1) {
            Serial.println("✅ WiFi 연결 성공");
            return;
        }

        // 실패 시 메시지 출력 및 3초 후 재시도
        Serial.println("연결 실패. 다시 시도합니다...");
        attempts++;
        delay(3000);
    }

    // 최대 시도 횟수 초과 시 실패 메시지 출력
    Serial.println("WiFi 연결 실패 (최대 10회 시도)");
}

// 서버에 GET 요청을 보내고 응답 본문 추출
String WiFiModule::getCommand(const String& path) {
    // TCP 연결 시작
    sendCommand("AT+CIPSTART=\"TCP\",\"" SERVER_IP "\"," + String(SERVER_PORT), 2000);

    // 전달받은 API 경로를 사용하여 GET 요청 생성
    String httpRequest =
        "GET " + path + " HTTP/1.1\r\n"
        "Host: " SERVER_IP "\r\n"
        "\r\n";

    // 요청 전송 준비 및 본문 출력
    sendCommand("AT+CIPSEND=" + String(httpRequest.length()), 1000);
    esp.print(httpRequest);

    // 응답 반환
    return readResponse();
}

// 서버에 POST 요청 전송 및 응답 수신
String WiFiModule::postData(const String& path, const String& json) {
    int contentLength = json.length();

    // TCP 연결
    sendCommand("AT+CIPSTART=\"TCP\",\"" SERVER_IP "\"," + String(SERVER_PORT), 2000);

    // 전달받은 경로를 기반으로 HTTP POST 요청 구성
    String httpRequest =
        "POST " + path + " HTTP/1.1\r\n"
        "Host: " SERVER_IP "\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + String(contentLength) + "\r\n\r\n" + json;

    // 요청 전송
    sendCommand("AT+CIPSEND=" + String(httpRequest.length()), 1000);
    esp.print(httpRequest);

    return readResponse();
}

/// -------------------------------------------------------------------------------------------------------------------
/// private 메서드
/// -------------------------------------------------------------------------------------------------------------------

void WiFiModule::sendCommand(const String& cmd, int delayMs) {
    esp.println(cmd);
    delay(delayMs);
}

String WiFiModule::readResponse() {
    String response = "";
    long timeout = millis() + 2000;

    while (millis() < timeout) {
        while (esp.available()) {
            response += static_cast<char>(esp.read());
        }
    }

    int idx = response.indexOf("\r\n\r\n");
    if (idx > 0) return response.substring(idx + 4);
    return "";
}