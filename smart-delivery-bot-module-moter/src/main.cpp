#include "SmartBot.h"
#include "Config.h"

// 함수 선언부 ===========================================================================================================
void modulsSetting();   // 모듈을 초기 설정 하는 함수입니다.
void waitAndHandle();   // CommLink로부터 수신된 명령을 파싱하고 SmartBot 동작 제어

// 객체 생성 =============================================================================================================
SmartBot bot(1, 2, A0, A1);  // 모터 포트: 1,2 / 센서: A0, A1

#include <SoftwareSerial.h>
SoftwareSerial comm(COMM_RX_PIN, COMM_TX_PIN);  // 수신 RX=2, 송신 TX=3

// 프로그램 설정 ==========================================================================================================
void setup() {
    modulsSetting();

    Serial.println("[DELIVERY BOT - SUB] 보조 모듈 준비 완료");
}

void loop() {
    waitAndHandle();    // 수신된 명령 처리
    bot.update();       // 라인트레이싱 동작
    delay(10);      // 반드시 넣기
}
// SET-UP FUNCTION =====================================================================================================

// 모듈을 초기 설정 하는 함수입니다.
void modulsSetting() {
    Serial.begin(115200);     // 디버깅용 시리얼
    comm.begin(BAUD_RATE);  // 통신 초기화
    bot.begin();            // 모터 및 센서 초기화

    Serial.println("[INFO] 모든 로컬 모듈 초기화 완료");
}


// LOOP FUNCTION =======================================================================================================

// CommLink로부터 수신된 명령을 파싱하고 SmartBot 동작 제어
// 명령 수신 + ACK 응답 + 명령 실행까지 하나로 처리
void waitAndHandle() {
    if (comm.available()) {
        String command = comm.readStringUntil('\n');
        command.trim();

        Serial.print("[수신] 명령어: ");
        Serial.println(command);

        // ACK 전송
        comm.println("ACK");

        if (command == "START") {
            Serial.println("[ACTION] 로봇 재시작");
        } else if (command == "STOP") {
            Serial.println("[ACTION] 로봇 정지");
        } else if (command == "TOGGLE") {
            Serial.println("[ACTION] 상태 전환");
            // 상태 토글 로직을 여기에 추가
        } else {
            Serial.println("[WARNING] 알 수 없는 명령");
        }
    }

    // 라인트레이싱, 기타 업데이트 등 필요시 여기에 추가
    delay(10);
}