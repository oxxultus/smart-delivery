#include <SoftwareSerial.h>   // 소프트웨어 시리얼 통신 라이브러리 (하드웨어 시리얼 외에 추가 시리얼 사용 시 필요)
#include <AFMotor.h>          // Adafruit 모터 쉴드용 제어 라이브러리

// 소프트웨어 시리얼 포트 설정 (RFID 리더기용)
// 12번 핀: RX (수신), 13번 핀: TX (송신)
SoftwareSerial rfidSerial(12, 13);

// 모터 객체 생성 (Adafruit Motor Shield)
// 모터 1번 포트 = 왼쪽 바퀴, 모터 4번 포트 = 오른쪽 바퀴
AF_DCMotor motor_L(1);
AF_DCMotor motor_R(4);

// RFID 관련 제어 변수
bool stopFlag = false;               // RFID "STOP" 명령을 수신했는지 여부
unsigned long stopStartTime = 0;     // 정지 시작 시각 (millis 단위로 기록)

void setup() {
  Serial.begin(9600);         // 시리얼 모니터와 통신용 (디버깅용)
  rfidSerial.begin(9600);     // RFID 리더기와의 소프트웨어 시리얼 통신 시작

  Serial.println("Eduino Smart Car Start!");  // 시동 메시지 출력

  // 모터 초기 설정: 속도 설정 후 정지 상태로 시작
  motor_L.setSpeed(230);      // 왼쪽 모터 속도 (0~255)
  motor_L.run(RELEASE);       // 모터 정지
  motor_R.setSpeed(230);      // 오른쪽 모터 속도
  motor_R.run(RELEASE);
}

void loop() {
  // ================================
  // 1. RFID 메시지 수신 및 정지 처리
  // ================================

  if (rfidSerial.available()) {                     // RFID 데이터가 수신되었는지 확인
    String msg = rfidSerial.readStringUntil('\n');  // 줄바꿈(\n)까지의 문자열 수신
    msg.trim();                                     // 앞뒤 공백 제거

    if (msg == "STOP" && !stopFlag) {               // STOP 명령 수신 && 아직 정지 중이 아니라면
      stopFlag = true;                              // 정지 상태로 전환
      stopStartTime = millis();                     // 현재 시각 저장
      stopMotors();                                 // 모터 정지
      Serial.println("RFID 감지됨! 20초간 정지");   // 알림 출력
    }
  }

  // ================================
  // 2. RFID로 인한 정지 시간 경과 확인
  // ================================
  if (stopFlag) {
    if (millis() - stopStartTime >= 20000) {        // 20초 경과 시
      stopFlag = false;                             // 정지 해제
      Serial.println("20초 경과, 주행 재개");       // 알림 출력
    } else {
      stopMotors();     // 아직 20초가 안 지났다면 계속 정지 상태 유지
      return;           // 이후 코드(센서 읽기 및 모터 제어)는 실행하지 않음
    }
  }

  // ================================
  // 3. 라인트레이서 센서 입력 처리
  // ================================

  int val1 = digitalRead(A0);   // 왼쪽 센서 (A0) 값 읽기
  int val2 = digitalRead(A5);   // 오른쪽 센서 (A5) 값 읽기

  // ================================
  // 4. 라인트레이서 주행 로직
  // ================================

  if (val1 == 0 && val2 == 0) {
    // 양쪽 센서 모두 검정 라인 감지 → 직진
    motor_L.run(FORWARD);
    motor_R.run(FORWARD);
  }
  else if (val1 == 0 && val2 == 1) {
    // 왼쪽 검정, 오른쪽 흰색 → 오른쪽 회전
    motor_L.run(FORWARD);
    motor_R.run(RELEASE);   // 오른쪽 바퀴 정지
  }
  else if (val1 == 1 && val2 == 0) {
    // 왼쪽 흰색, 오른쪽 검정 → 왼쪽 회전
    motor_L.run(RELEASE);   // 왼쪽 바퀴 정지
    motor_R.run(FORWARD);
  }
  else if (val1 == 1 && val2 == 1) {
    // 양쪽 모두 흰색 (라인 없음) → 정지
    stopMotors();
  }
}

// ================================
// [모터 정지 함수]
// 양쪽 바퀴 모두 정지
// ================================
void stopMotors() {
  motor_L.run(RELEASE);
  motor_R.run(RELEASE);
}