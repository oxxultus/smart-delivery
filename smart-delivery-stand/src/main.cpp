#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>
#include <Arduino.h>
#include "WiFiModule/WiFiModule.h"

// RFID 핀
#define SS_PIN 10
#define RST_PIN 9

// Stepper 모터 핀
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

// 모터 설정
const int stepsPerRevolution = 1800; // 1회전 스텝 수 (28BYJ-48 기준)
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// RFID 리더 객체
MFRC522 rfid(SS_PIN, RST_PIN);

// 모터 작동 상태 변수
bool motorActive = false;
unsigned long motorStartTime = 0;
const unsigned long motorRunDuration = 10000; // 5초

void sendStartVehicleCommand(WiFiModule& wifi, const String& vehicleId) {
    String jsonData = "{\"command\": \"start_vehicle\", \"vehicle_id\": \"" + vehicleId + "\"}";
    String response = wifi.postData("/vehicle/start", jsonData);
    Serial.println("서버 응답: " + response);
}

WiFiModule wifi;

const String ssid = "1234";
const String password = "4321";
const String serverIp = "http://localhost:8080";

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("RFID + 스텝모터 테스트 시작");

    myStepper.setSpeed(10); // 10 RPM

    // 와이파이 연결
    Serial.println("Wifi 연결 시도");
    wifi.begin(ssid, password);
}

void loop() {

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        // 새 카드가 감지되면
        if (!motorActive && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
            Serial.print("카드 UID: ");
            for (byte i = 0; i < rfid.uid.size; i++) {
                Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
                Serial.print(rfid.uid.uidByte[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            Serial.println("✅ 카드 인식됨! 모터 작동 시작");

            motorActive = true;
            motorStartTime = millis();

            rfid.PICC_HaltA();         // 카드 통신 종료
            rfid.PCD_StopCrypto1();    // 암호화 종료
            delay(100);               // 중복 인식 방지
        }

        // 모터 작동 시간 동안 회전
        if (motorActive) {
            myStepper.step(stepsPerRevolution / 10); // 천천히 회전

            if (millis() - motorStartTime >= motorRunDuration) {
                motorActive = false;
                Serial.println("⏹ 모터 작동 종료");
            }
        }
        sendStartVehicleCommand(wifi, "carID");
    }
}
