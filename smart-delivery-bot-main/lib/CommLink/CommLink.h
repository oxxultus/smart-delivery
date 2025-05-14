#ifndef COMMLINK_H
#define COMMLINK_H

#include <Arduino.h>
#include <SoftwareSerial.h>

/**
 * @class CommLink
 * @brief 두 아두이노 간의 양방향 문자열 기반 통신을 위한 통합 클래스
 *        SoftwareSerial을 사용하여 문자열 전송과 수신, ACK 기반 응답 처리 지원
 *
 * 주요 기능:
 * - sendWithAck(): 메시지를 전송하고 ACK 수신 여부를 판단
 * - waitAndAck(): 수신된 메시지에 대해 ACK 자동 응답
 * - sendAck(): 수동 ACK 응답 전송
 * - receiveLine(): 줄 단위 문자열 수신
 * - sendLine(): 줄 단위 문자열 전송
 */
class CommLink {
private:
    SoftwareSerial serial;      ///< SoftwareSerial 인스턴스 (TX, RX 핀 사용자 정의)
    uint16_t timeoutMs = 2000;  ///< 메시지 전송 후 ACK 수신을 기다릴 최대 시간 (밀리초 단위)

public:
    /**
     * @brief 생성자
     * @param rxPin 수신에 사용할 핀 번호 (아두이노 기준)
     * @param txPin 송신에 사용할 핀 번호
     */
    CommLink(uint8_t rxPin, uint8_t txPin);

    /**
     * @brief 시리얼 통신 시작
     * @param baudRate 통신 속도 (예: 9600)
     */
    void begin(long baudRate);

    /**
     * @brief 메시지를 전송하고, ACK 응답을 받을 때까지 대기
     * @param message 전송할 문자열 (자동으로 '\n'이 추가됨)
     * @return true: ACK 수신 성공 / false: 시간 초과 등으로 실패
     */
    bool sendWithAck(const String& message);

    /**
     * @brief 수신된 메시지를 읽고, 자동으로 ACK 응답 전송
     *        수신된 메시지는 시리얼 모니터에 출력됨
     */
    void waitAndAck();

    /**
     * @brief 수신 응답으로 'ACK' 문자열을 전송
     *        sendWithAck()에서 사용됨
     */
    void sendAck();

    /**
     * @brief 수신된 데이터가 있는지 확인
     * @return true: 읽을 데이터가 있음 / false: 없음
     */
    bool hasLine();

    /**
     * @brief '\n'까지의 문자열을 수신
     * @return 수신된 문자열 (줄바꿈 미포함)
     */
    String receiveLine();

    /**
     * @brief 문자열을 전송하며 자동 줄바꿈('\n') 추가
     * @param text 전송할 문자열
     */
    void sendLine(const String& text);
};

#endif