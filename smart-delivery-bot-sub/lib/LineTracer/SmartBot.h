#ifndef SMARTBOT_H
#define SMARTBOT_H

#include <Arduino.h>
#include <AFMotor.h>

/**
 * @class SmartBot
 * @brief 라인트레이싱 기반의 스마트 주행 로봇 제어 클래스
 *
 * - 모터 2개 (AFMotor)
 * - 라인 센서 2개
 * - 주행 상태 isRunning 추적
 * - pause/resume/toggle 기능 지원
 */
class SmartBot {
private:
    AF_DCMotor motorL;
    AF_DCMotor motorR;
    uint8_t sensorL;
    uint8_t sensorR;
    bool isRunning = false;  ///< 현재 주행 상태

public:
    SmartBot(uint8_t m1, uint8_t m2, uint8_t sL, uint8_t sR);

    void begin();       //< 모터 및 센서 초기화
    void update();      //< 주행 로직 실행
    void pause();       //< 정지 상태로 전환
    void resume();      //< 주행 시작
    void toggle();      //< 주행 상태 반전
    bool running() const; //< 현재 주행 상태 반환

private:
    void stopMotors();  ///< 모터 정지
};

#endif