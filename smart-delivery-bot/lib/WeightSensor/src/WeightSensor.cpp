#include "WeightSensor.h"

WeightSensor::WeightSensor(uint8_t dout, uint8_t clk, float factor)
    : calibrationFactor(factor), doutPin(dout), clkPin(clk) { }

void WeightSensor::begin() {
    Serial.println("HX711 scale INIT");

    scale.begin(doutPin, clkPin);
    scale.set_scale(calibrationFactor);
    scale.tare(); // 영점 조정
    Serial.println("WeightSensor ready.");
}

void WeightSensor::printWeight() {
    float grams = getWeight();
    Serial.print("무게 측정: ");
    Serial.print(grams, 1);
    Serial.println(" g");
}

float WeightSensor::getWeight() {
    return scale.get_units();
}

long WeightSensor::getRawReading() {
    return scale.read_average();
}

void WeightSensor::calibrate(float knownWeight) {
    Serial.println("보정 중... 기준 무게: " + String(knownWeight) + "g");

    long raw = scale.read_average(10); // 여러 번 평균
    if (knownWeight > 0) {
        calibrationFactor = (float)raw / knownWeight;
        scale.set_scale(calibrationFactor);
        Serial.print("새 보정값 적용됨: ");
        Serial.println(calibrationFactor, 4);
    } else {
        Serial.println("기준 무게는 0보다 커야 합니다.");
    }
}

float WeightSensor::getCalibrationFactor() const {
    return calibrationFactor;
}
