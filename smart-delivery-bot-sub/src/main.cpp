#include "CommLink.h"

CommLink comm(2, 3);

void setup() {
    Serial.begin(9600);
    comm.begin(9600);
    Serial.println("수신기 시작");
}

void loop() {
    if (comm.hasLine()) {
        String msg = comm.receiveLine();
        msg.trim();
        Serial.print("수신된 명령: ");
        Serial.println(msg);

        comm.sendAck();  // ACK 응답

        // 명령어 분기 처리
        if (msg == "/작동") {
            Serial.println("작동 시작!");
            // 여기에 작동 관련 로직 실행
            // 예: 모터 시작, LED 켜기 등
        } else if (msg == "/정지") {
            Serial.println("정지 명령 수신됨");
            // 여기에 정지 관련 로직 실행
            // 예: 모터 정지, LED 끄기 등
        } else {
            Serial.println("알 수 없는 명령어");
        }
    }
}