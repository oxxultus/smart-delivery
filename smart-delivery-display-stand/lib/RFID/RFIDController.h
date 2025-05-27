#ifndef RFIDCONTROLLER_H
#define RFIDCONTROLLER_H

#include <Arduino.h>
#include <MFRC522.h>

/**
 * [RFIDController 클래스]
 * - MFRC522 기반 RFID 리더기 초기화 및 감지 기능을 담당
 * - 감지 후 일정 시간 동안 재감지를 방지 (쿨다운)
 * - RFID 감지 시 지정된 출력 스트림을 통해 "STOP" 명령 전송
 */
class RFIDController {
public:
    /**
     * 생성자
     * @param ssPin        RFID SS(SDA) 핀
     * @param rstPin       RFID RST 핀
     * @param debugSerial  디버깅용 시리얼 (Serial 등)
     * @param outputSerial 외부 장치로 명령을 보낼 출력 시리얼 (SoftwareSerial 등)
     */
    RFIDController(uint8_t ssPin, uint8_t rstPin, HardwareSerial &debugSerial, Stream &outputSerial);

    // RFID 모듈 초기화 (SPI 시작 및 리더기 초기화)
    void begin();

    // 주기적으로 호출하여 RFID 감지 및 STOP 명령 전송
    boolean update();

private:
    MFRC522 rfid;                // MFRC522 객체
    HardwareSerial &debug;      // 디버깅용 시리얼 참조
    Stream &out;                // 명령 출력용 시리얼 참조
    unsigned long lastDetectedTime;  // 마지막 RFID 감지 시간 (ms 기준)
    const unsigned long cooldown = 20000; // 재감지까지의 대기시간 (20초)
};

#endif