#ifndef WEIGHTSENSOR_H
#define WEIGHTSENSOR_H

#include <Arduino.h>
#include "HX711.h"

class WeightSensor {
private:
    HX711 scale;
    float calibrationFactor;
    uint8_t doutPin;
    uint8_t clkPin;

public:
    WeightSensor(uint8_t dout, uint8_t clk, float factor);
    void begin();
    void printWeight();
    float getWeight();
    long getRawReading();

    /**
     * @brief 기준 무게(knownWeight g)를 올린 후 호출하여 자동 보정
     */
    void calibrate(float knownWeight);

    /**
     * @brief 현재 보정 계수 반환
     */
    float getCalibrationFactor() const;
};

#endif //WEIGHTSENSOR_H