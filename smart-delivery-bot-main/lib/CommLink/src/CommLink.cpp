#include "CommLink.h"

#if defined(ESP32)
CommLink::CommLink(HardwareSerial& hwSerial, int rx, int tx)
    : serial(&hwSerial), rxPin(rx), txPin(tx) {}

void CommLink::begin(long baudRate) {
    serial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
}
#else
CommLink::CommLink(uint8_t rx, uint8_t tx) {
    serial = new SoftwareSerial(rx, tx);
}

void CommLink::begin(long baudRate) {
    serial->begin(baudRate);
}
#endif

// ✅ println() 사용으로 단일 메시지 전송 보장
void CommLink::sendLine(const String& text) {
    serial->println(text);     // print() + '\n' 대신 println()
    serial->flush();           // 출력 버퍼 전송 완료까지 대기 (중요)
}

// ✅ '\n'까지 수신
String CommLink::receiveLine() {
    return serial->readStringUntil('\n');
}

// 데이터 수신 가능 여부 확인
bool CommLink::hasLine() {
    return serial->available() > 0;
}

// 메시지 전송 후 ACK 대기
bool CommLink::sendWithAck(const String& message) {
    sendLine(message);
    unsigned long start = millis();
    while (millis() - start < timeoutMs) {
        if (hasLine()) {
            String response = receiveLine();
            response.trim();
            if (response == "ACK") return true;
        }
    }
    return false;
}

// 메시지 수신 시 ACK 전송
void CommLink::waitAndAck() {
    if (hasLine()) {
        String msg = receiveLine();
        msg.trim();
        Serial.print("수신됨: ");
        Serial.println(msg);
        sendAck();  // ACK 응답
    }
}

// ACK 전송 (println)
void CommLink::sendAck() {
    sendLine("ACK");
}