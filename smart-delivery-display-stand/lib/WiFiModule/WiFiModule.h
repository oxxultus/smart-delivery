//
// Created by 영진 on 25. 5. 13.
//

#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <SoftwareSerial.h>

/**
 * @brief WiFiModule 클래스는 ESP-01 (또는 AT 명령 기반 ESP 모듈)을 사용하여
 *        WiFi 네트워크에 연결하고, 서버와의 HTTP 통신(GET/POST)을 수행하는 기능을 제공합니다.
 */
class WiFiModule {
private:
    SoftwareSerial esp;  // ESP-01과 통신할 SoftwareSerial (TX=3, RX=2)

    /**
     * @brief ESP 모듈에 AT 명령어를 전송한 후 일정 시간(ms) 동안 응답을 기다립니다.
     * @param cmd 전송할 AT 명령 문자열
     * @param delayMs 명령 후 대기 시간(ms)
     */
    void sendCommand(const String& cmd, int delayMs);

    /**
     * @brief ESP로부터 수신된 데이터를 읽어 문자열로 반환합니다.
     *        HTTP 응답의 헤더는 제외하고 본문만 추출하여 리턴합니다.
     * @return 응답 본문 문자열
     */
    String readResponse();

public:
    /**
     * @brief 생성자: SoftwareSerial 객체를 초기화합니다.
     *        RX는 2번 핀, TX는 3번 핀으로 고정되어 있습니다.
     */
    WiFiModule();

    /**
     * @brief WiFi 네트워크에 연결을 시도합니다.
     *        최대 10회까지 재시도하며, 성공 시 연결 완료 메시지를 출력합니다.
     * @param ssid 연결할 WiFi SSID
     * @param password 해당 WiFi의 비밀번호
     */
    void begin(const String& ssid, const String& password);

    /**
     * @brief 지정한 API 경로로 HTTP GET 요청을 전송하고, 응답 본문을 반환합니다.
     * @param path GET 요청을 보낼 서버 경로 (예: "/command")
     * @return 서버로부터 수신한 응답 본문 문자열
     */
    String getCommand(const String& path);

    /**
     * @brief 지정한 API 경로로 HTTP POST 요청을 전송하고, 응답 본문을 반환합니다.
     * @param path POST 요청을 보낼 서버 경로 (예: "/receive")
     * @param json 전송할 JSON 형식의 본문 데이터
     * @return 서버로부터 수신한 응답 본문 문자열
     */
    String postData(const String& path, const String& json);
};

#endif // WIFI_MODULE_H