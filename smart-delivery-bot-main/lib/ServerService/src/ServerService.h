#ifndef WIFI_WEB_SERVICE_H
#define WIFI_WEB_SERVICE_H

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <functional>
#include <WString.h>

/**
 * WebService 클래스
 * - HTTP GET/POST 요청 수신 처리 (서버 역할)
 * - HTTP GET/POST 요청 전송 (클라이언트 역할)
 */
class ServerService {
private:
    int serverPort;                          // HTTP 서버 포트 (복사 저장)
    ESP8266WebServer server;                 // 내장 웹 서버 인스턴스

    // 라우팅 핸들러 콜백 함수들
    std::function<void()> startHandler = nullptr;
    std::function<void()> goHandler = nullptr;
    std::function<void()> stopHandler = nullptr;
    std::function<void(const String&)> postHandler = nullptr;

    // 내부 메서드
    void setupRoutes();       // 핸들러 라우팅 등록

public:
    explicit ServerService(int serverPort);    // 포트 번호를 값으로 전달받음

    void begin();             // 서버 시작
    void handle();            // 클라이언트 요청 처리 (loop에서 호출)

    // 핸들러 등록 메서드
    void setStartHandler(const std::function<void()> &handler);                 // /start GET
    void setGoHandler(const std::function<void()> &handler);                    // /go GET
    void setStopHandler(const std::function<void()> &handler);                  // /stop GET
    void setPostHandler(const std::function<void(const String&)> &handler);     // /post POST

    // 외부 서버로 HTTP 요청 전송
    static String sendGETRequest(const char* host, uint16_t port, const String& pathWithParams);
    static String sendPostRequest(const char* host, uint16_t port, const String& path, const JsonDocument& jsonDoc);

    // 내부 서버 핸들러 등록 여부 확인 함수
    [[nodiscard]] bool isStartHandlerSet() const;
    [[nodiscard]] bool isGoHandlerSet() const;
    [[nodiscard]] bool isStopHandlerSet() const;
};

#endif // WIFI_WEB_SERVICE_H