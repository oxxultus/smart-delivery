#include <Arduino.h>
#include <HardwareSerial.h>
#include <Stepper.h>

// 한 바퀴에 필요한 스텝 수 (28BYJ-48 기준: 2048)
const int stepsPerRevolution = 2048;

// 스텝 모터 객체 생성 (IN1, IN2, IN3, IN4 핀 순서)
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
    // 모터 속도 설정 (RPM)
    myStepper.setSpeed(15);
    Serial.begin(9600);
    Serial.println("Stepper Motor Test 시작");
}

void loop() {
    // 정방향 회전
    Serial.println("정방향 회전 중...");
    myStepper.step(stepsPerRevolution); // 한 바퀴
    delay(1000);

    // 역방향 회전
    Serial.println("역방향 회전 중...");
    myStepper.step(-stepsPerRevolution); // 반대 방향 한 바퀴
    delay(1000);
}