#include "CommLink.h"

/**
 * @brief 생성자
 * @details SoftwareSerial 객체를 생성하여 통신 핀 지정
 * @param rxPin 수신에 사용할 아두이노 핀 번호
 * @param txPin 송신에 사용할 아두이노 핀 번호
 */
CommLink::CommLink(uint8_t rxPin, uint8_t txPin)
    : serial(rxPin, txPin) {}

/**
 * @brief 소프트웨어 시리얼 통신 초기화
 * @param baudRate 통신 속도 (예: 9600bps)
 */
void CommLink::begin(long baudRate) {
    serial.begin(baudRate);
}

/**
 * @brief 문자열을 줄 단위로 전송
 * @details 텍스트를 전송한 뒤 줄바꿈 문자('\n')를 자동으로 추가함
 * @param text 전송할 문자열
 */
void CommLink::sendLine(const String& text) {
    serial.print(text);
    serial.print('\n');
}

/**
 * @brief 줄 단위 문자열 수신
 * @details '\n' 문자가 나올 때까지 수신하며, 포함하지 않고 반환
 * @return 수신된 문자열
 */
String CommLink::receiveLine() {
    return serial.readStringUntil('\n');
}

/**
 * @brief 수신된 데이터가 있는지 확인
 * @details 수신 버퍼에 줄 단위 데이터가 들어왔는지 확인
 * @return true: 데이터 있음 / false: 없음
 */
bool CommLink::hasLine() {
    return serial.available() > 0 && serial.peek() != -1;
}

/**
 * @brief 메시지를 전송하고, 일정 시간 내 ACK 응답을 수신 대기
 * @details 줄 단위로 메시지를 전송한 후 "ACK"라는 문자열을 기다림
 * @param message 전송할 문자열
 * @return true: ACK 수신 성공 / false: 타임아웃 등 실패
 */
bool CommLink::sendWithAck(const String& message) {
    sendLine(message);  // 메시지 전송

    unsigned long startTime = millis();  // 시작 시간 기록
    while (millis() - startTime < timeoutMs) {
        if (hasLine()) {
            String response = receiveLine();
            response.trim();
            if (response == "ACK") return true;  // 응답 성공
        }
    }
    return false;  // 타임아웃
}

/**
 * @brief 수신된 메시지를 처리하고, 자동으로 ACK 응답 전송
 * @details 수신된 메시지를 시리얼 모니터에 출력하고 "ACK"를 전송
 */
void CommLink::waitAndAck() {
    if (hasLine()) {
        String msg = receiveLine();
        msg.trim();  // 양쪽 공백 제거
        Serial.print("수신됨: ");
        Serial.println(msg);
        sendAck();  // 응답 전송
    }
}

/**
 * @brief "ACK" 문자열을 전송
 * @details 응답용으로 사용됨. sendWithAck()에서 기대하는 응답
 */
void CommLink::sendAck() {
    sendLine("ACK");
}