#include <SoftwareSerial.h>   // 소프트웨어 시리얼 통신용 라이브러리
#include <SPI.h>              // SPI 통신을 위한 라이브러리 (MFRC522 모듈 사용 시 필수)
#include <MFRC522.h>          // RFID RC522 모듈 제어용 라이브러리

// MFRC522 모듈 핀 정의
#define SS_PIN  10            // SPI 슬레이브 선택 (SDA) 핀
#define RST_PIN 9             // 리셋 핀

// MFRC522 객체 생성
MFRC522 rfid(SS_PIN, RST_PIN);

// 다른 아두이노와 통신할 소프트웨어 시리얼 객체 (TX=12, RX=13)
SoftwareSerial mySerial(12, 13);  // RFID → Smart Car 신호 전송용

// RFID 감지 시 타이머 변수
unsigned long lastDetectedTime = 0;      // 마지막으로 태그를 감지한 시간
const unsigned long cooldown = 20000;    // 쿨다운 타이머: 20초 (단위: ms)

void setup() {
  Serial.begin(9600);        // 디버깅용 기본 시리얼 통신 시작
  mySerial.begin(9600);      // 소프트웨어 시리얼 통신 시작 (Smart Car 측에서 수신)
  SPI.begin();               // SPI 통신 초기화
  rfid.PCD_Init();           // RFID 리더 초기화
  Serial.println("RFID 리더기 시작됨");  // 초기화 메시지
}

void loop() {
  // ================
  // [1] 쿨다운 확인
  // ================
  // 마지막 감지 후 20초가 지나지 않았으면 무시 (재감지 방지)
  if ((millis() - lastDetectedTime) < cooldown) {
    return;
  }

  // =============================
  // [2] RFID 카드 감지 시도
  // =============================

  // 새로운 카드가 감지되지 않았거나, 카드 읽기에 실패한 경우 → 아무 작업 안함
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // =============================
  // [3] 카드가 감지된 경우
  // =============================

  Serial.println("RFID 감지됨!");   // 시리얼 모니터에 메시지 출력
  mySerial.println("STOP");        // 스마트카 측으로 "STOP" 명령 전송
  lastDetectedTime = millis();     // 현재 시간을 마지막 감지 시간으로 기록

  // 카드 통신 종료 (리더기 대기 상태로 전환)
  rfid.PICC_HaltA();               // 카드와의 통신 종료
  rfid.PCD_StopCrypto1();          // 암호화 종료
}