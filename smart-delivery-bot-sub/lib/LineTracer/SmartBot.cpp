#include "SmartBot.h"
#include <Arduino.h>

/**
 * SmartBot 생성자
 * @param motorL_num 왼쪽 모터 번호 (AFMotor 모듈 기준)
 * @param motorR_num 오른쪽 모터 번호
 * @param leftSensorPin 왼쪽 센서 입력 핀 번호 (A0 등)
 * @param rightSensorPin 오른쪽 센서 입력 핀 번호 (A5 등)
 */
SmartBot::SmartBot(uint8_t motorL_num, uint8_t motorR_num, int leftSensorPin, int rightSensorPin)
    : motor_L(motorL_num), motor_R(motorR_num),
      sensor_L_pin(leftSensorPin), sensor_R_pin(rightSensorPin),
      isPaused(false) {}


/**
 * 초기 설정: 시리얼 통신 시작, 모터 속도 설정 및 정지
 */
void SmartBot::begin() {
    Serial.begin(9600);
    Serial.println("Smart Bot Start!");

    motor_L.setSpeed(170);  // 왼쪽 모터 속도 설정
    motor_R.setSpeed(170);  // 오른쪽 모터 속도 설정

    stop();  // 초기에는 정지 상태로 시작
}


/**
 * 주행 상태 업데이트 함수
 * 센서값을 읽고, 라인트레이싱 로직에 따라 방향을 결정
 */
void SmartBot::update() {
    if (isPaused) return;  // 일시정지 상태면 아무것도 하지 않음

    int val_L = digitalRead(sensor_L_pin);  // 왼쪽 센서 읽기
    int val_R = digitalRead(sensor_R_pin);  // 오른쪽 센서 읽기

    if (val_L == 0 && val_R == 0) {
        forward();        // 두 센서 모두 검정 선 → 직진
    } else if (val_L == 0 && val_R == 1) {
        turnRight();      // 왼쪽은 검정, 오른쪽은 흰색 → 오른쪽 회전
    } else if (val_L == 1 && val_R == 0) {
        turnLeft();       // 왼쪽은 흰색, 오른쪽은 검정 → 왼쪽 회전
    } else {
        stop();           // 둘 다 흰색 → 정지
    }
}


/**
 * 전진 동작: 양쪽 모터 전진
 */
void SmartBot::forward() {
    motor_L.run(FORWARD);
    motor_R.run(FORWARD);
}


/**
 * 좌회전 동작: 왼쪽 모터 정지, 오른쪽 모터 전진
 */
void SmartBot::turnLeft() {
    motor_L.run(RELEASE);
    motor_R.run(FORWARD);
}


/**
 * 우회전 동작: 왼쪽 모터 전진, 오른쪽 모터 정지
 */
void SmartBot::turnRight() {
    motor_L.run(FORWARD);
    motor_R.run(RELEASE);
}


/**
 * 정지 동작: 양쪽 모터 모두 정지
 */
void SmartBot::stop() {
    motor_L.run(RELEASE);
    motor_R.run(RELEASE);
}


/**
 * 주행 일시 정지: isPaused 플래그 설정 및 모터 정지
 */
void SmartBot::pause() {
    isPaused = true;
    stop();
    Serial.println("Paused");
}


/**
 * 주행 재개: isPaused 플래그 해제
 */
void SmartBot::resume() {
    isPaused = false;
    Serial.println("Resumed");
}