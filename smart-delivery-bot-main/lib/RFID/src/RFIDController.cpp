#include "RFIDController.h"
#include <SPI.h>

// 생성자
RFIDController::RFIDController(uint8_t ssPin, uint8_t rstPin)
    : rfid(ssPin, rstPin) {}

// RFID 리더기 초기화 (Serial 객체 전달)
void RFIDController::begin(HardwareSerial &debugSerial) {
    debug = &debugSerial;
    SPI.begin();
    rfid.PCD_Init();
    debug->println("[RFID] RFID 리더기 초기화 완료");
}

// RFID 태그 감지 시 UID 문자열 반환
String RFIDController::getUID() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return ""; // 카드 감지 안됨
    }

    String uidStr = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
        uidStr += String(rfid.uid.uidByte[i], HEX);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return uidStr;
}