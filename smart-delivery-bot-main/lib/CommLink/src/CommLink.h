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
#if defined(ESP32)
    HardwareSerial* serial;   // ESP32용 하드웨어 시리얼
    int rxPin;
    int txPin;
#else
    SoftwareSerial* serial;   // AVR용 소프트웨어 시리얼
#endif

    uint16_t timeoutMs = 2000;

public:
#if defined(ESP32)
    CommLink(HardwareSerial& hwSerial, int rx, int tx);
#else
    CommLink(uint8_t rx, uint8_t tx);
#endif

    void begin(long baudRate);
    void sendLine(const String& text);
    String receiveLine();
    bool hasLine();
    bool sendWithAck(const String& message);
    void waitAndAck();
    void sendAck();
};

#endif // COMMLINK_H