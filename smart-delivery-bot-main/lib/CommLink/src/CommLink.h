#ifndef COMMLINK_H
#define COMMLINK_H

#include <Arduino.h>

class CommLink {
private:
    HardwareSerial& serial;
    uint16_t timeoutMs = 2000;
    int rxPin, txPin;

public:
    CommLink(HardwareSerial& hwSerial, int rx, int tx);
    void begin(long baudRate);

    bool sendWithAck(const String& message);
    void waitAndAck();
    void sendAck();

    bool hasLine();
    String receiveLine();
    void sendLine(const String& text);

    Stream& getSerial();
};

#endif