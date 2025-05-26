#include <functional>
#include <WString.h>
#include "ServerService.h"

// ========== 생성자: WiFi 정보와 포트 설정, 서버 인스턴스 초기화 =================================================================
ServerService::ServerService(const int serverPort)
    : serverPort(serverPort), server(serverPort) {}

// ========== 서버 시작: WiFi 연결 후 라우팅 등록 및 서버 시작 ===================================================================
void ServerService::begin() {
    setupRoutes();
    server.begin();
    Serial.println("[ServerService] Delivery BOT의 내장 HTTP 서버가 시작되었습니다.");
}

// 클라이언트 요청을 처리 (loop에서 반복 호출 필요)
void ServerService::handle() {
    server.handleClient();
}

// ========== 핸들러 등록 메서드 ===========================================================================================
void ServerService::setStartHandler(const std::function<void()> &handler) {
    startHandler = handler;
}

void ServerService::setGoHandler(const std::function<void()> &handler) {
    goHandler = handler;
}

void ServerService::setStopHandler(const std::function<void()> &handler) {
    stopHandler = handler;
}

void ServerService::setPostHandler(const std::function<void(const String&)> &handler) {
    postHandler = handler;
}

// ========== 클라이언트 요청 전송 ==========================================================================================
String ServerService::sendGETRequest(const char* host, const uint16_t port, const String& pathWithParams) {
    String response = "";
    WiFiClient client;

    if (client.connect(host, port)) {
        client.print(String("GET ") + pathWithParams + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");

        unsigned long timeout = millis() + 3000;
        while (client.connected() && millis() < timeout) {
            while (client.available()) {
                response += (char)client.read();
            }
        }
        client.stop();
    }

    return response;
}

String ServerService::sendPostRequest(const char* host, uint16_t port, const String& path, const JsonDocument& jsonDoc) {
    String response = "";
    WiFiClient client;
    if (client.connect(host, port)) {
        // JSON 데이터 직렬화
        String jsonString;
        serializeJson(jsonDoc, jsonString);

        // POST 요청 전송
        client.print(String("POST ") + path + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + jsonString.length() + "\r\n\r\n" +
                     jsonString);

        // 응답 수신
        while (client.connected() || client.available()) {
            if (client.available()) {
                response += client.readStringUntil('\n');
            }
        }
        client.stop();
    }
    return response;
}

// ========== 요청 라우팅 등록 =============================================================================================
void ServerService::setupRoutes() {
    // GET: /start
    if (startHandler) {
        server.on("/start", HTTP_GET, [this]() {
            startHandler();
            server.send(200, "application/json", "{\"message\":\"Handled GET /start\"}");
        });
    }

    // GET: /go
    if (goHandler) {
        server.on("/go", HTTP_GET, [this]() {
            goHandler();
            server.send(200, "application/json", "{\"message\":\"Handled GET /go\"}");
        });
    }

    // GET: /stop
    if (stopHandler) {
        server.on("/stop", HTTP_GET, [this]() {
            stopHandler();
            server.send(200, "application/json", "{\"message\":\"Handled GET /stop\"}");
        });
    }

    // POST: /post
    if (postHandler) {
        server.on("/post", HTTP_POST, [this]() {
            String body = server.arg("plain"); // 요청 본문 추출
            postHandler(body);
            server.send(200, "application/json", "{\"message\":\"Handled POST /post\"}");
        });
    }
}

// 내장 서버 핸들러 등록 함수 ================================================================================================
[[nodiscard]] bool ServerService::isStartHandlerSet() const {
    return static_cast<bool>(startHandler);
}

[[nodiscard]] bool ServerService::isGoHandlerSet() const {
    return static_cast<bool>(goHandler);
}

[[nodiscard]] bool ServerService::isStopHandlerSet() const {
    return static_cast<bool>(stopHandler);
}