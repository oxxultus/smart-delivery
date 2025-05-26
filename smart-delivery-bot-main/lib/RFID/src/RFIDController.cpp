#include "RFIDController.h"
#include <SPI.h>

// 생성자
RFIDController::RFIDController(uint8_t ssPin, uint8_t rstPin)
    : rfid(ssPin, rstPin), debug(nullptr) {}

// 디버깅 없이 초기화
void RFIDController::begin() {
    SPI.begin();        // 기본 SPI 핀으로 시작 (VSPI: SCK=18, MISO=19, MOSI=23)
    rfid.PCD_Init();    // RFID 초기화
}

// 디버깅 시리얼과 함께 초기화
void RFIDController::begin(HardwareSerial &debugSerial) {
    debug = &debugSerial;
    SPI.begin();        // SPI 시작
    rfid.PCD_Init();    // RFID 초기화
    if (debug) debug->println("[RFID] RFID 리더기 초기화 완료");
}

// RFID 태그 감지 시 UID 반환
String RFIDController::getUID() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return "";  // 태그 감지 안됨
    }

    String uidStr;
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";  // 앞자리가 한 자리면 0 붙이기
        uidStr += String(rfid.uid.uidByte[i], HEX);
    }

    uidStr.toLowerCase();  // 소문자로 통일

    rfid.PICC_HaltA();        // 태그 통신 종료
    rfid.PCD_StopCrypto1();   // 암호화 종료

    if (debug) debug->println("[RFID] 감지된 UID: " + uidStr);

    return uidStr;
}