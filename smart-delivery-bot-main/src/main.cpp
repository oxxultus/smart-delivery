#include "CommLink.h"

CommLink comm(2, 3);  // RX, TX

void setup() {
    Serial.begin(9600);
    comm.begin(9600);
    Serial.println("송신기 시작");

    delay(1000);  // 초기 통신 안정화
}

void loop() {
    bool ok = comm.sendWithAck("/작동");  // 또는 "/정지"

    if (ok) {
        Serial.println("명령 전송 성공");
    } else {
        Serial.println("명령 전송 실패");
    }

    delay(5000);
}