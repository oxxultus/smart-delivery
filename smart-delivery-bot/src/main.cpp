#include <Arduino.h>
#include "Config.h"

#include "CommLink.h"
#include "ServerService.h"
#include "RFIDController.h"
#include "WiFiConnector.h"
// #include "WeightSensor.h"

#include "model/PaymentData.h" // 구조체, 클래스

// 함수 선언부 ===========================================================================================================
bool sendWithRetry(const String& cmd, const int retries = 3);       // [UTILITY-1] 명령 전송 함수 (재시도 포함)
void simpleMessage(String message);                                 // [UTILITY-2] 간편 메시지 사용 메서드
bool isAdminCard(const String& uid);                                // [LOOP-1] 관리자 카드 여부 판별
bool refreshPaymentData(int maxRetries = 3);                        // [LOOP-2] 결제 내역 초기화 및 재요청 로직
bool fetchPaymentDataUntilSuccess(const int count);                 // [LOOP-3] 외부 서버로 GET 요청 전송해 결제 내역을 받아온다.
void handleMatchedProduct(const String& matchedName);               // [LOOP-4] 상품 매칭 시 동작을 처리하는 함수
void checkDetectedUid();                                            // [LOOP-5] UID를 인식해서 결제내역 확인 하는 함수
void modulsSetting();                                               // [SETUP-1] 모듈을 초기 설정 하는 함수입니다.
void setServerHandler();                                            // [SETUP-2] 핸들러 등록을 진행하는 함수입니다.
// bool isWeightChanged();                 // 이전 무게와 현재 무게의 차이를 비교 판단 하는 함수 입니다.

// 객체 생성 =============================================================================================================
WiFiConnector wifi(WIFI_SSID, WIFI_PASSWORD);                 // WiFiConnect 객체 생성
ServerService serverService(INNER_SERVER_PORT);                    // WebService 객체 생성
RFIDController rfidController(RC_SDA, RC_RST);                  // RFID 리더기 설정 (SDA, RST)
// WeightSensor weightSensor(HX_DOUT, HX_CLK, HX_FACTOR);                   // 무게 센서 설정
PaymentData payment;                                                        // 결제 내역 저장

// 프로그램 설정 및 시작 ====================================================================================================
void setup() {
    modulsSetting();            // 1. 일반 모듈 초기 설정
    setServerHandler();         // 2. 내장 서버 API 핸들러 메서드 등록
    serverService.begin();      // 3. 내장 서버 구동 설정
    Serial.println("[DELIVERY BOT][MAIN] 메인 모듈 준비 완료"); simpleMessage("종료선");  // 함수: [UTILITY-2]
}

void loop() {
    serverService.handle();     // 1. 내장 서버 구동
    checkDetectedUid();         // 2. UID를 인식해서 결제내역 확인 하는 함수
    delay(1);                   // 3. WDT 리셋 방지
}

// SETUP FUNCTION =====================================================================================================

// [SETUP-1] 모듈을 초기 설정 하는 함수입니다.
void modulsSetting() {
    Serial.begin(SERIAL_BAUDRATE);   // 시리얼 설정

    // 함수: [UTILITY-2]
    simpleMessage("시작선");
    if (USE_RFID) {
        rfidController.begin(Serial); // RFID 리더기 초기화
    } else {
        Serial.println("[INFO] RFID 리더기 비활성화됨 (하드웨어 없음)");
    }
    // weightSensor.begin();             // weightSensor 초기화
    Serial2.begin(SERIAL2_BAUDRATE);

    wifi.connect();             // wifi 연결
}

// [SETUP-2] 핸들러 등록을 진행하는 함수입니다.
void setServerHandler() {
    // 핸들러 등록
    serverService.setStartHandler([]() {
        Serial.println("[ServerService][GET /start] 로봇 시작 명령 수신");

        // 결제 내역이 없으면 수신 시도
        if (payment.getPaymentId() == "") {
            Serial.println("[ServerService] 결제 내역 없음 → 새로 요청");
            const bool result = fetchPaymentDataUntilSuccess(5);
            if (!result || payment.getPaymentId() == "") {
                Serial.println("[ServerService][BLOCKED] 서버에 결제 내역 없음 → 시작 차단됨");
                return;
            }
        }
        // 결제 내역이 이미 존재하거나 새로 수신 성공한 경우
        Serial.println("[ServerService][START] 결제 내역이 존재함 → 로봇 시작");
        sendWithRetry("START"); // 함수: [UTILITY-1]
    });
    serverService.setGoHandler([]() {
        Serial.println("[ServerService][GET /go] 로봇 이동 명령 수신");
        sendWithRetry("GO"); // 함수: [UTILITY-1]
    });
    serverService.setStopHandler([]() {
        Serial.println("[ServerService][GET /stop] 로봇 정지 명령 수신");
        sendWithRetry("STOP"); // 함수: [UTILITY-1]
    });

    Serial.println("[setServerHandler][1/2] 내장 서버 API 실행 함수 등록 확인 절차 시작");
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
    Serial.println("[setServerHandler][2/2] 내장 서버 API 실행 함수 등록 절차 완료\n");
}

// LOOP FUNCTION =======================================================================================================

// [LOOP-1] 관리자 카드 여부 판별
bool isAdminCard(const String& uid) {
    return uid == ADMIN_UID || uid == MASTER_KEY;
}

// [LOOP-2] 결제 내역 초기화 및 재요청 로직
bool refreshPaymentData(int maxRetries) {
    Serial.println("\n[RFIDController][[2/3] 관리자 카드 감지됨 → 결제 내역 초기화");
    payment.clear();

    if (!fetchPaymentDataUntilSuccess(maxRetries)) {
        Serial.println("[ServerService][PaymentData][404] " + String(maxRetries) + "회 시도하였지만 결제내역 가져오는데 실패했습니다. 재시도 하려면 카드를 다시 찍어주세요.");
        return false;
    }
    return true;
}

// [LOOP-3] 외부 서버로 GET 요청 전송해 결제 내역을 받아온다.
bool fetchPaymentDataUntilSuccess(const int count) {
    int i = 0;
    while ( i < count) {
        Serial.println("[ServerService][PaymentData][1/3] 결제 내역을 가져오는 중입니다..");
        String getResponse = ServerService::sendGETRequest(MAIN_SERVER_IP, MAIN_SERVER_PORT, GET_PAYMENT);
        //Serial.println(getResponse);

        String responseBody = getResponse.substring(getResponse.indexOf("\r\n\r\n") + 4);

        if (payment.parseFromJson(responseBody)) {
            Serial.println("[ServerService][PaymentData][2/3] 가져온 결제 내역을 출력합니다.");
            // Serial.println("[ServerService][INFO] 결제 ID: " + payment.getPaymentId());
            // Serial.println("[ServerService][INFO] 결제 상품 목록:");
            payment.printItems();
            Serial.println("[ServerService][PaymentData][3/3] 결제 내역 수신 성공. 다음 단계로 진행합니다.");Serial.println("");
            return true;
        }

        Serial.println("[ServerService][재시도] 결제 내역 파싱 실패. 2초 후 재시도...");Serial.println("");
        delay(3000); i++;
    }
    return false;
}

// [LOOP-4] 상품 매칭 시 동작을 처리하는 함수
void handleMatchedProduct(const String& matchedName) {
    Serial.println("[RFIDController][2/3] 일치하는 상품: " + matchedName + " → 모터 정지 명령 전송");
    // 함수: [UTILITY-1]
    if (sendWithRetry("STOP")) {
        Serial.println("[RFIDController][3/3] STOP 명령 전송 및 ACK 수신 성공");
    } else {
        Serial.println("[RFIDController][3/3] STOP 명령 전송 실패 (ACK 없음)");
    }

    Serial.println("[RFIDController][3/3] 다음 상품으로 이동 합니다.\n");
}

// [LOOP-5] UID를 인식해서 결제내역 확인 하는 함수
void checkDetectedUid() {
    String detectedUid = rfidController.getUID();
    if (detectedUid.isEmpty()) return;

    Serial.println("[RFIDController][1/3] 감지된 UID: " + detectedUid);

    // 함수: [LOOP-2], [LOOP-3], [LOOP-4]
    if (isAdminCard(detectedUid)) {
        refreshPaymentData(); // 기본 3회 시도
        return;
    }

    // 함수 [LOOP-5]
    String matchedName;
    if (payment.matchUID(detectedUid, matchedName)) {
        handleMatchedProduct(matchedName);
    } else {
        Serial.println("[RFIDController][2/3] 감지된 UID는 결제 내역에 없음 → 무시");
        Serial.println("[RFIDController][3/3] 다음 상품으로 이동 합니다.\n");
    }
}

// UTILITY FUNCTION ====================================================================================================

// [UTILITY-1] 명령 전송 함수 (재시도 포함)
bool sendWithRetry(const String& cmd, const int retries) {
    for (int i = 0; i < retries; ++i) {
        Serial2.println(cmd);  // 명령 전송
        Serial.println("[Wired Comm][Serial2][1/2] " + cmd + " 명령 전송");

        unsigned long start = millis();
        while (millis() - start < 1000) {  // 1초 이내 응답 대기
            if (Serial2.available()) {
                String response = Serial2.readStringUntil('\n');
                response.trim();
                if (response == "ACK") {
                    Serial.println("[Wired Comm][Serial2][2/2] ACK 수신 성공");
                    return true;
                } else {
                    Serial.println("[Wired Comm][Serial2][2/2]  잘못된 응답: " + response);
                }
            }
        }

        Serial.println("[Wired Comm][Serial2][RETRY]  ACK 수신 실패, 재시도 " + String(i + 1) + "\n");
        delay(200);
    }

    Serial.println("[Wired Comm][4/4]  " + cmd + " 명령 전송 실패 (ACK 없음)\n");
    return false;
}

// [UTILITY-2] 간편 메시지 사용 메서드
void simpleMessage(String message) {
    if (message == "시작선") {
        Serial.println("= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =");
    }else if (message == "종료선") {
        Serial.println("= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =");
    }
}

// 이전 무게와 현재 무게의 차이를 비교 판단 하는 함수 입니다.
/* bool isWeightChanged(const float thresholdGram) {
    static float lastWeight = 0.0;  // 이전 측정값 저장 (초기 0)
    float currentWeight = weightSensor.getWeight();  // 현재 무게 측정
    float diff = abs(currentWeight - lastWeight);    // 변화량 계산

    if (diff >= thresholdGram) {
        lastWeight = currentWeight;  // 변화 감지 → 기준값 갱신
        return true;
    }
    return false;
}
*/

