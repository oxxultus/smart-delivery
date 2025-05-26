#include "CommLink.h"

#if defined(ESP32)

// ESP32용: HardwareSerial을 외부에서 주입하며 핀도 설정
CommLink::CommLink(HardwareSerial& hw, int rx, int tx)
    : hwSerial(&hw), rxPin(rx), txPin(tx) {
    // 핀 설정 및 스트림 지정은 begin()에서 처리
}

void CommLink::begin(long baudRate) {
    if (hwSerial && rxPin >= 0 && txPin >= 0) {
        hwSerial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
        stream = hwSerial;
    }
}

#else

// AVR용: 내부 SoftwareSerial 객체 생성
CommLink::CommLink(uint8_t rxPin, uint8_t txPin) {
    swSerial = new SoftwareSerial(rxPin, txPin);
    stream = swSerial;
}

void CommLink::begin(long baudRate) {
    if (swSerial) swSerial->begin(baudRate);
}

#endif

void CommLink::sendLine(const String& text) {
    stream->print(text);
    stream->print('\n');
}

String CommLink::receiveLine() {
    return stream->readStringUntil('\n');
}

bool CommLink::hasLine() {
    return stream->available() > 0 && stream->peek() != -1;
}

bool CommLink::sendWithAck(const String& message) {
    sendLine(message);
    Serial.println("[CommLink] 전송된 메시지: "+ message);
    unsigned long startTime = millis();
    while (millis() - startTime < timeoutMs) {
        if (hasLine()) {
            String response = receiveLine();
            Serial.println(response);
            response.trim();
            if (response == "ACK") return true;
        }
    }
    return false;
}

void CommLink::waitAndAck() {
    if (hasLine()) {
        String msg = receiveLine();
        msg.trim();
        Serial.print("수신됨: ");
        Serial.println(msg);
        sendAck();
    }
}

void CommLink::sendAck() {
    sendLine("ACK");
}
