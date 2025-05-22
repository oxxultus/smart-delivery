#include "CommLink.h"
#include "SmartBot.h"
#include "Config.h"

// 함수 선언부 ===========================================================================================================
void modulsSetting();   // 모듈을 초기 설정 하는 함수입니다.
void handleCommand();   // CommLink로부터 수신된 명령을 파싱하고 SmartBot 동작 제어

// 객체 생성 =============================================================================================================
CommLink comm(COMM_RX_PIN, COMM_TX_PIN);
SmartBot bot(1, 2, A0, A1);  // 모터 포트: 1,2 / 센서: A0, A1

// 프로그램 설정 ==========================================================================================================
void setup() {
    modulsSetting();

    Serial.println("[DELIVERY BOT - SUB] 보조 모듈 준비 완료");
}

void loop() {
    comm.waitAndAck();  // 명령 수신 및 ACK 응답
    handleCommand();    // 수신된 명령 처리
    bot.update();       // 라인트레이싱 동작
}
// SET-UP FUNCTION =====================================================================================================

// 모듈을 초기 설정 하는 함수입니다.
void modulsSetting() {
    Serial.begin(9600);     // 디버깅용 시리얼
    comm.begin(BAUD_RATE);  // 통신 초기화
    bot.begin();            // 모터 및 센서 초기화

    Serial.println("[INFO] 모든 로컬 모듈 초기화 완료");
}


// LOOP FUNCTION =======================================================================================================

// CommLink로부터 수신된 명령을 파싱하고 SmartBot 동작 제어
void handleCommand() {
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
}