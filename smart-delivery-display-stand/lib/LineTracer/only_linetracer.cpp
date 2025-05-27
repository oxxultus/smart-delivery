#include <SoftwareSerial.h>   // 소프트웨어 시리얼 통신 라이브러리 (이 코드에서는 사용되지 않음)
#include <AFMotor.h>          // Adafruit 모터 쉴드 제어 라이브러리

// 왼쪽 바퀴에 연결된 모터 객체 생성 (모터 포트 1번 사용)
AF_DCMotor motor_L(1);

// 오른쪽 바퀴에 연결된 모터 객체 생성 (모터 포트 4번 사용)
AF_DCMotor motor_R(4);

void setup() {
  Serial.begin(9600);         // PC(시리얼 모니터)와의 통신 속도 설정 (9600bps)
  Serial.println("Eduino Smart Car Start!");  // 시리얼 모니터에 초기 메시지 출력

  // 왼쪽 모터 속도 설정 (최대 255 중 230)
  motor_L.setSpeed(230);
  // 초기 상태에서는 모터 정지
  motor_L.run(RELEASE);

  // 오른쪽 모터 속도 설정
  motor_R.setSpeed(230);
  motor_R.run(RELEASE);
}

void loop() {
  // 라인트레이서 센서 값 읽기
  // A0: 왼쪽 센서, A5: 오른쪽 센서
  // 검정 라인을 감지하면 0(LOW), 흰색 바탕이면 1(HIGH)을 반환하는 방식이라고 가정
  int val1 = digitalRead(A0);    // 왼쪽 센서 값
  int val2 = digitalRead(A5);    // 오른쪽 센서 값

  // 센서 기반 방향 제어
  if (val1 == 0 && val2 == 0) {
    // 양쪽 센서 모두 검정 라인을 감지한 경우: 직진
    motor_L.run(FORWARD);
    motor_R.run(FORWARD);
  }
  else if (val1 == 0 && val2 == 1) {
    // 왼쪽 센서만 검정 라인을 감지한 경우: 우회전
    motor_L.run(FORWARD);   // 왼쪽 바퀴 전진
    motor_R.run(RELEASE);   // 오른쪽 바퀴 정지
  }
  else if (val1 == 1 && val2 == 0) {
    // 오른쪽 센서만 검정 라인을 감지한 경우: 좌회전
    motor_L.run(RELEASE);   // 왼쪽 바퀴 정지
    motor_R.run(FORWARD);   // 오른쪽 바퀴 전진
  }
  else if (val1 == 1 && val2 == 1) {
    // 양쪽 센서 모두 흰색 배경일 경우: 정지
    motor_L.run(RELEASE);
    motor_R.run(RELEASE);
  }
}