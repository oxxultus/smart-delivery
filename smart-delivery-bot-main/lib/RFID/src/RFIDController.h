#ifndef RFIDCONTROLLER_H
#define RFIDCONTROLLER_H

#include <Arduino.h>
#include <MFRC522.h>

/**
 * @class RFIDController
 * @brief MFRC522 기반 RFID 리더기 제어 클래스
 *
 * 기능:
 * - SPI 초기화 및 RFID 리더기 설정
 * - RFID 태그 감지 시 UID를 문자열로 반환
 */
class RFIDController {
public:
    /**
     * @brief 생성자
     * @param ssPin RFID SS(SDA) 핀
     * @param rstPin RFID RST 핀
     */
    RFIDController(uint8_t ssPin, uint8_t rstPin);

    /**
     * @brief RFID 리더기 초기화
     * @param debugSerial 디버깅용 시리얼 (Serial)
     */
    void begin(HardwareSerial &debugSerial);

    /**
     * @brief RFID 태그가 감지되면 UID 문자열 반환
     * @return UID 문자열 (예: "a1b2c3d4"), 감지되지 않으면 ""
     */
    String getUID();

private:
    MFRC522 rfid;
    HardwareSerial* debug = nullptr;
};

#endif // RFIDCONTROLLER_H