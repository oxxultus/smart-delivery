#include "SmartBot.h"

/**
 * @brief SmartBot 생성자
 * @details 주어진 모터 포트와 센서 핀을 저장하고, AF_DCMotor 인스턴스를 초기화함
 * @param m1 왼쪽 모터 포트 번호 (AFMotor 기준)
 * @param m2 오른쪽 모터 포트 번호
 * @param sL 왼쪽 라인 센서 아날로그 핀 번호
 * @param sR 오른쪽 라인 센서 아날로그 핀 번호
 */
SmartBot::SmartBot(uint8_t m1, uint8_t m2, uint8_t sL, uint8_t sR)
    : motorL(m1), motorR(m2), sensorL(sL), sensorR(sR) {}

/**
 * @brief SmartBot 초기화 함수
 * @details 모터 속도 및 센서 핀모드를 설정하고, 초기 정지 상태로 전환함
 */
void SmartBot::begin() {
    motorL.setSpeed(200);     // 왼쪽 모터 속도 설정
    motorR.setSpeed(200);     // 오른쪽 모터 속도 설정
    motorL.run(RELEASE);      // 모터 정지
    motorR.run(RELEASE);
    pinMode(sensorL, INPUT);  // 센서 입력 모드 설정
    pinMode(sensorR, INPUT);
}

/**
 * @brief 라인트레이싱 주행 제어 함수
 * @details 주행 상태일 경우 센서 값을 읽어 라인을 따라 움직임
 *          정지 상태일 경우 아무 동작도 하지 않음
 */
void SmartBot::update() {
    if (!isRunning) return;  // 정지 상태면 주행 로직 스킵

    int left = digitalRead(sensorL);
    int right = digitalRead(sensorR);

    // 두 센서 모두 검정 선 감지 → 직진
    if (left == 0 && right == 0) {
        motorL.run(FORWARD);
        motorR.run(FORWARD);
    }
    // 오른쪽만 흰색 → 왼쪽으로 회전
    else if (left == 0 && right == 1) {
        motorL.run(FORWARD);
        motorR.run(RELEASE);
    }
    // 왼쪽만 흰색 → 오른쪽으로 회전
    else if (left == 1 && right == 0) {
        motorL.run(RELEASE);
        motorR.run(FORWARD);
    }
    // 둘 다 흰색 → 정지
    else {
        stopMotors();
    }
}

/**
 * @brief 정지 명령
 * @details 모터를 멈추고 주행 상태를 false로 설정함
 */
void SmartBot::pause() {
    stopMotors();
    isRunning = false;
}

/**
 * @brief 재시작 명령
 * @details 주행 상태를 true로 설정하여 update()가 주행을 다시 시작하게 함
 */
void SmartBot::resume() {
    isRunning = true;
}

/**
 * @brief 상태 전환 명령
 * @details 주행 상태일 경우 멈추고, 정지 상태일 경우 재시작
 */
void SmartBot::toggle() {
    if (isRunning) pause();
    else resume();
}

/**
 * @brief 현재 주행 상태 조회
 * @return true면 주행 중, false면 정지 상태
 */
bool SmartBot::running() const {
    return isRunning;
}

/**
 * @brief 모터를 완전히 멈추는 내부 유틸리티 함수
 */
void SmartBot::stopMotors() {
    motorL.run(RELEASE);
    motorR.run(RELEASE);
}