#include "RFIDController.h"
#include <SPI.h>

/**
 * RFIDController 생성자
 */
RFIDController::RFIDController(uint8_t ssPin, uint8_t rstPin, HardwareSerial &debugSerial, Stream &outputSerial)
  : rfid(ssPin, rstPin), debug(debugSerial), out(outputSerial), lastDetectedTime(0) {}

/**
 * RFID 리더기 초기화 함수
 */
void RFIDController::begin() {
    SPI.begin();         // SPI 시작
    rfid.PCD_Init();     // RFID 초기화
    debug.println("RFID 리더기 시작됨");
}

/**
 * 매 루프마다 호출되는 update 함수
 * RFID 태그가 감지되었는지 확인하고, 일정 시간 내 중복 감지는 무시
 * 감지되면 외부 장치로 "STOP" 메시지를 송신하고 true 반환
 * 감지되지 않았거나 쿨다운 중이면 false 반환
 */
boolean RFIDController::update() {
    // [1] 쿨다운 시간 체크
    if ((millis() - lastDetectedTime) < cooldown) {
        return false;
    }

    // [2] 카드 감지 여부 확인
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return false;
    }

    // [3] 카드 감지 시 동작
    debug.println("RFID 감지됨!");
    out.println("STOP");              // 외부 장치로 명령 전송
    lastDetectedTime = millis();      // 시간 기록

    // [4] 카드 통신 종료
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    return true;  // 감지 성공
}

loop() {
    boolean 결제내역존재여부 = false;
    String 결재내역;

    while (true){
        // "get" 요청으로 서버에서 결제 완료가된 목록이 있는지 계속해서 확인한다
        // 만약 결제내역이 존재한다면 결제내역을 가져온 뒤 결제내역이 존재한다고 결재내역 = true 처리한다.
        // 이후 결제 내역을 받아와서 "결제내역" 에 결제 상품 리스트를 저장한다.
        // 이후 break; 문을 통해 반복문을 진행한다.
    }
    // 만약 결재내역 정보의 RFID의 키가 찍히면 잠시 대기한다.
    // 서버로 전송하여 해당 RFID의 자판기에 명령을 전달한다.
    // 무게 센서로 상품이 들어와 변경이 학인되면 카트를 출발 시킨다.

}

/*
동작 흐름 (RFID 카드가 자판기에 있음)

1. 결제 및 RFID 키 등록
    •	사용자가 서버에서 상품을 결제함
    •	서버는 결제된 상품의 RFID 키 목록을 카트에 전달

2. 카트가 자판기에 접근
    •	카트는 자판기에 접근하여 RFID 리더기로 자판기의 카드(RFID 키)를 읽음

3. 카트가 결제 내역과 비교
    •	읽은 RFID 키가 자신이 보유한 결제 내역에 포함되어 있는지 확인
    •	해당 키가 존재하면 카트를 멈춤 (정지 시간: 지정된 딜레이 예: 5초~20초)

4. 카트가 서버에 상품 요청
    •	카트는 서버에:
    •	“자판기에서 읽은 RFID 키”
    •	“해당 키에 대한 요청 수량”
을 포함하여 요청

5. 서버가 자판기로 명령 전송
    •	서버는 자판기에:
    •	해당 상품의 수량을 포함한 배출 명령 전송

6. 자판기 상품 배출
    •	자판기는 명령을 수신하여 (지속적인 반복문을 사용해서 get요청으로확인 해야됨, 현제 보유한 모듈로는 제한적)
    •	혹은 서버 기능을 수행할 수 있는 모듈을 사용해야 함.
    •	해당 RFID 키에 대응하는 상품을 지정된 수량만큼 배출
*/