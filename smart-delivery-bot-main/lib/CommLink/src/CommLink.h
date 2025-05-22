#ifndef COMMLINK_H
#define COMMLINK_H

#include <Arduino.h>
#include <SoftwareSerial.h>

/**
 * @class CommLink
 * @brief 두 아두이노 간의 양방향 문자열 기반 통신을 위한 통합 클래스
 */
class CommLink {
private:
    SoftwareSerial serial;      ///< SoftwareSerial 인스턴스 (TX, RX 핀 사용자 정의)
    uint16_t timeoutMs = 2000;  ///< 메시지 전송 후 ACK 수신을 기다릴 최대 시간 (밀리초 단위)

public:
    CommLink(uint8_t rxPin, uint8_t txPin);
    void begin(long baudRate);

    bool sendWithAck(const String& message);
    void waitAndAck();
    void sendAck();

    bool hasLine();
    String receiveLine();
    void sendLine(const String& text);

    /**
     * @brief 내부 시리얼 스트림 접근자 (예: RFIDController에서 사용)
     * @return SoftwareSerial 참조
     */
    Stream& getSerial();  // <-- 추가됨
};

#endif