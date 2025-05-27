#ifndef RFIDCONTROLLER_H
#define RFIDCONTROLLER_H

#include <Arduino.h>
#include <MFRC522.h>

/**
 * @class RFIDController
 * @brief MFRC522 기반 RFID 리더기 제어 클래스
 */
class RFIDController {
public:
    /**
     * @brief 생성자
     * @param ssPin RFID SS(SDA) 핀 (예: GPIO5)
     * @param rstPin RFID RST 핀 (예: GPIO22)
     */
    RFIDController(uint8_t ssPin, uint8_t rstPin);

    /**
     * @brief RFID 리더기 초기화 (디버깅 없이 사용)
     */
    void begin();

    /**
     * @brief RFID 리더기 초기화 (디버깅용 Serial 포트 지정)
     * @param debugSerial Serial 출력용 객체 (예: Serial)
     */
    void begin(HardwareSerial &debugSerial);

    /**
     * @brief RFID 태그 감지 시 UID 반환
     * @return 감지된 UID 문자열, 감지되지 않으면 빈 문자열 ("")
     */
    String getUID();

private:
    MFRC522 rfid;                  // MFRC522 객체
    HardwareSerial* debug = nullptr;  // 디버깅 출력용 Serial 포인터
};

#endif // RFIDCONTROLLER_H