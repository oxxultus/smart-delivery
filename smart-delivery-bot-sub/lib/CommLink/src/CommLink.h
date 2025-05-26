#ifndef COMMLINK_H
#define COMMLINK_H

#include <Arduino.h>

#if defined(ESP32)
  #include <HardwareSerial.h>
#else
  #include <SoftwareSerial.h>
#endif

class CommLink {
private:
    Stream* stream = nullptr;

#if defined(ESP32)
    HardwareSerial* hwSerial = nullptr;
    int rxPin = -1;
    int txPin = -1;
#else
    SoftwareSerial* swSerial = nullptr;
#endif

    uint16_t timeoutMs = 2000;

public:
    // ESP32용 생성자 (핀 포함)
#if defined(ESP32)
    CommLink(HardwareSerial& hwSerial, int rx, int tx);
#else
    CommLink(uint8_t rxPin, uint8_t txPin);  // Uno용
#endif

    void begin(long baudRate);
    void sendLine(const String& text);
    String receiveLine();
    bool hasLine();
    bool sendWithAck(const String& message);
    void waitAndAck();
    void sendAck();
};

#endif
