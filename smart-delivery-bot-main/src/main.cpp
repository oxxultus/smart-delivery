#include <Arduino.h>
#include "Config.h"

#include "CommLink.h"
#include "ServerService.h"
#include "RFIDController.h"
#include "WiFiConnector.h"
#include "WeightSensor.h"

#include "model/PaymentData.h" // 구조체, 클래스

// 함수 선언부 ===========================================================================================================
bool sendWithRetry(const String& cmd, const int retries = 3); // 명령 전송 함수 (재시도 포함)
void setServerHandler();                // 핸들러 등록을 진행하는 함수 입니다.
void fetchPaymentDataUntilSuccess();    // 외부 서버로 GET 요청 전송해 결제 내역을 받아온다.
void modulsSetting();                   // 일반 모듈을 초기 설정 하는 함수 입니다.
void checkDetectedUid();                // RFID - UID를 확인 후 결제 내역 체크 하는 함수 입니다.
bool isWeightChanged();                 // 이전 무게와 현재 무게의 차이를 비교 판단 하는 함수 입니다.

// 객체 생성 =============================================================================================================
WiFiConnector wifi(WIFI_SSID, WIFI_PASSWORD);                 // WiFiConnect 객체 생성
ServerService serverService(INNER_SERVER_PORT);                    // WebService 객체 생성
RFIDController rfidController(RC_SDA, RC_RST);                  // RFID 리더기 설정 (SDA, RST)
WeightSensor weightSensor(HX_DOUT, HX_CLK, HX_FACTOR);       // 무게 센서 설정

HardwareSerial commSerial(2);                                       // UART2
CommLink comm(commSerial, COMM_RX_PIN, COMM_TX_PIN);              // RX = GPIO16, TX = GPIO17

PaymentData payment;                                                        // 결제 내역

// 프로그램 설정 ==========================================================================================================
void setup() {
    // 일반 모듈 초기 설정
    modulsSetting();

    // 네트워크 연결 및 내장 서버 설정
    wifi.connect();                 // 1. wifi 연결
    fetchPaymentDataUntilSuccess(); // 2. 외부 서버로 GET 요청 전송, 결제 내역을 받아온다.
    setServerHandler();             // 3. 내장 서버 API 핸들러 메서드 등록
    serverService.begin();          // 4. 내장 서버 구동 설정

    Serial.println("[DELIVERY BOT - MAIN] 메인 모듈 준비 완료");
}

void loop() {
    serverService.handle();  // 내장 서버 구동

    checkDetectedUid(); // UID를 인식해서 결제내역 확인 하는 함수

    delay(1);  // WDT 리셋 방지
}

// SET-UP FUNCTION =====================================================================================================

// 모듈을 초기 설정 하는 함수입니다.
void modulsSetting() {
    Serial.begin(SERIAL_BAUDRATE);   // 시리얼 설정

    if (USE_RFID) {
        rfidController.begin(Serial); // RFID 리더기 초기화
        Serial.println("[INFO] RFID 리더기 사용 중");
    } else {
        Serial.println("[INFO] RFID 리더기 비활성화됨 (하드웨어 없음)");
    }
    weightSensor.begin();                  // weightSensor 초기화
    comm.begin(9600);             // comm 객체 통신 시리얼 설정

    Serial.println("[INFO] 모든 로컬 모듈 초기화 완료");
}

// 명령 전송 함수 (재시도 포함)
bool sendWithRetry(const String& cmd, const int retries) {
    for (int i = 0; i < retries; ++i) {
        if (comm.sendWithAck(cmd)) {
            Serial.println("[CommLink] " + cmd + " 명령 전송 성공 (ACK 수신)");
            return true;
        }
        delay(200);  // 재시도 간 딜레이
    }
    Serial.println("[CommLink] " + cmd + " 명령 전송 실패 (ACK 없음)");
    return false;
}

// 핸들러 등록을 진행하는 함수입니다.
void setServerHandler() {
    // 핸들러 등록
    serverService.setStartHandler([]() {
        Serial.println("[ServerService][GET /start] 로봇 시작 명령 수신");

        if (payment.getPaymentId() != "") {
            Serial.println("[ServerService][START] 결제 내역이 존재함 → 로봇 시작");
            sendWithRetry("START");
        } else {
            Serial.println("[ServerService][BLOCKED] 결제 내역 없음 → 시작 차단됨");
        }
    });
    serverService.setGoHandler([]() {
        Serial.println("[ServerService][GET /go] 로봇 이동 명령 수신");
        sendWithRetry("GO");
    });
    serverService.setStopHandler([]() {
        Serial.println("[ServerService][GET /stop] 로봇 정지 명령 수신");
        sendWithRetry("STOP");
    });

    Serial.println("[setServerHandler] 내장 서버 API 실행 함수 등록 확인 절차 시작 = = = = = = = = = = = = = = = = = = = = = = =");
        auto printHandlerStatus = [](const char* name, bool status) {
            Serial.print("[");
            Serial.print(status ? "\u2714" : "\u2718"); // ✔ 또는 ✘
            Serial.print("] ");
            Serial.print(name);
            Serial.println(" 핸들러 등록 " + String(status ? "완료" : "실패"));
        };
        printHandlerStatus("/start", serverService.isStartHandlerSet());
        printHandlerStatus("/go",    serverService.isGoHandlerSet());
        printHandlerStatus("/stop",  serverService.isStopHandlerSet());
    Serial.println("[setServerHandler] 내장 서버 API 실행 함수 등록 절차 완료 = = = = = = = = = = = = = = = = = = = = = = = = =");
}

// 외부 서버로 GET 요청 전송해 결제 내역을 받아온다. (결제 내역 수신 시도 → 성공할 때까지 반복)
void fetchPaymentDataUntilSuccess() {
    while (true) {
        String getResponse = ServerService::sendGETRequest(MAIN_SERVER_IP, MAIN_SERVER_PORT, GET_PAYMENT);
        Serial.println("[ServerService][GET 응답] = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =");
        Serial.println(getResponse);

        String responseBody = getResponse.substring(getResponse.indexOf("\r\n\r\n") + 4);

        if (payment.parseFromJson(responseBody)) {
            // Serial.println("[ServerService][INFO] 결제 ID: " + payment.getPaymentId());
            // Serial.println("[ServerService][INFO] 결제 상품 목록:");
            payment.printItems();
            Serial.println("[ServerService][SUCCESS] 결제 내역 수신 성공. 다음 단계로 진행합니다.");
            Serial.println("= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =");
            break;
        }

        Serial.println("[ServerService][RETRY] 결제 내역 파싱 실패. 3초 후 재시도...");
        delay(3000);
    }
}


// LOOP FUNCTION =======================================================================================================

// UID를 인식해서 결제내역 확인 하는 함수
void checkDetectedUid() {
    String detectedUid = rfidController.getUID();
    if (detectedUid.isEmpty()) return;

    Serial.println("[RFIDController] 감지된 UID: " + detectedUid);

    // 관리자 초기화 로직
    if (detectedUid == ADMIN_UID) {
        Serial.println("[ADMIN] 관리자 카드 감지됨 → 결제 내역 초기화");

        // 결제 내역 초기화
        payment.clear();

        // 결제 내역 다시 요청
        fetchPaymentDataUntilSuccess();

        Serial.println("[ADMIN] 새로운 결제 내역 수신 완료");
        // TODO: 필요하다면 초기 자리로 돌아가는 로직이 있으면 좋아보인다.
        return;  // 관리자 로직 이후 일반 로직 생략
    }

    String matchedName;
    if (payment.matchUID(detectedUid, matchedName)) {
        Serial.println("[RFIDController] 일치하는 상품: " + matchedName + " → 모터 정지 명령 전송");

        // 모터 정지 명령 전송 (예: 다른 아두이노에 STOP 명령 전송)
        if (comm.sendWithAck("STOP")) {
            Serial.println("[CommLink] STOP 명령 전송 성공 (ACK 수신)");
            //TODO: STOP 명령 수신 시 진열대(모듈)에 “상품 투입” 명령을 내려야 함
            // 1. → 메인 서버에 GET 요청으로 상품 정보 전달 (카트)
            // 2. → 메인 서버는 해당 상품을 임시 변수에 저장 (메인 서버)
            // 3. → 진열대는 서버에 지속적으로 GET 요청하여 처리할 상품 있는지 확인 (진열대)
            // 4. → 상품 처리 완료 후 서버에 완료 GET 전송 (진열대)
            // 5. → 서버는 카트 로봇에게 다음 동작 명령 전송 (메인 서버)
        } else {
            Serial.println("[CommLink] STOP 명령 전송 실패 (ACK 없음)");
        }
    } else {
        Serial.println("[RFIDController] 감지된 UID는 결제 내역에 없음 → 무시");
    }
}

// 이전 무게와 현재 무게의 차이를 비교 판단 하는 함수 입니다.
bool isWeightChanged(const float thresholdGram) {
    static float lastWeight = 0.0;  // 이전 측정값 저장 (초기 0)
    float currentWeight = weightSensor.getWeight();  // 현재 무게 측정
    float diff = abs(currentWeight - lastWeight);    // 변화량 계산

    if (diff >= thresholdGram) {
        lastWeight = currentWeight;  // 변화 감지 → 기준값 갱신
        return true;
    }
    return false;
}
