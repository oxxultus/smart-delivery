#ifndef SMART_BOT_H
#define SMART_BOT_H

#include <AFMotor.h>

class SmartBot {
private:
    AF_DCMotor motor_L;
    AF_DCMotor motor_R;
    int sensor_L_pin;
    int sensor_R_pin;
    bool isPaused;

public:
    SmartBot(uint8_t motorL_num, uint8_t motorR_num, int leftSensorPin, int rightSensorPin);

    void begin();
    void update();

    void forward();
    void turnLeft();
    void turnRight();
    void stop();

    void pause();
    void resume();
};

#endif // SMART_BOT_H