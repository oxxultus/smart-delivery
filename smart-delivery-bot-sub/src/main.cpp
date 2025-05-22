#include "CommLink.h"
#include "SmartBot.h"
#include "Config.h"

#define BAUD_RATE 9600

// 📌 인스턴스 선언
CommLink comm(COMM_RX_PIN, COMM_TX_PIN);
SmartBot bot(1, 2, A0, A1);  // 모터 포트: 1,2 / 센서: A0, A1

void setup() {
    Serial.begin(9600);     // 디버깅용 시리얼
    comm.begin(BAUD_RATE);  // 통신 초기화
    bot.begin();            // 모터 및 센서 초기화

    Serial.println("[DELIVERY BOT] 보조 모듈 준비 완료");
}

void loop() {
    comm.waitAndAck();  // 명령 수신 및 ACK 응답

    // 명령 파싱
    if (comm.hasLine()) {
        String command = comm.receiveLine();
        command.trim();

        Serial.print("명령 수신: ");
        Serial.println(command);

        if (command == "START") {
            bot.resume();
        } else if (command == "STOP") {
            bot.pause();
        } else if (command == "TOGGLE") {
            bot.toggle();
        } else {
            Serial.println("⚠️ 알 수 없는 명령");
        }
    }

    // 주행 업데이트
    bot.update();
}