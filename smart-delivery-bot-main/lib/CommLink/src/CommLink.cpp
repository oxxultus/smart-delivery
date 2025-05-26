#include "CommLink.h"

CommLink::CommLink(HardwareSerial& hwSerial, int rx, int tx)
    : serial(hwSerial), rxPin(rx), txPin(tx) {}

void CommLink::begin(long baudRate) {
    serial.begin(baudRate, SERIAL_8N1, rxPin, txPin);
}

void CommLink::sendLine(const String& text) {
    serial.print(text);
    serial.print('\n');
}

String CommLink::receiveLine() {
    return serial.readStringUntil('\n');
}

bool CommLink::hasLine() {
    return serial.available() > 0 && serial.peek() != -1;
}

bool CommLink::sendWithAck(const String& message) {
    sendLine(message);
    unsigned long startTime = millis();
    while (millis() - startTime < timeoutMs) {
        if (hasLine()) {
            String response = receiveLine();
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

Stream& CommLink::getSerial() {
    return serial;
}