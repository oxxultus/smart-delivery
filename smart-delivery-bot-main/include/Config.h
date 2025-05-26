#ifndef CONFIG_H
#define CONFIG_H
// ┌─────────────────────────────────────────────┐
// │                   기기 설정                   │
// └─────────────────────────────────────────────┘

// WiFi 정보
#define WIFI_SSID     "Kzz22e [2G]"
#define WIFI_PASSWORD "kyj@&3127415"
//#define WIFI_SSID     "iptime"
//#define WIFI_PASSWORD ""

// 메인 서버 IP 및 포트
#define MAIN_SERVER_IP   "221.161.182.186"
#define MAIN_SERVER_PORT 8080

// 내부 서버 포트 설정
#define INNER_SERVER_PORT 8081

// 메인 서버 결제내역 API 요청 URL
#define GET_PAYMENT "/bot/payment"

// 시리얼 속도 설정
#define SERIAL_BAUDRATE 115200
#define SERIAL2_BAUDRATE 9600

// 모듈 사용 설정
#define USE_RFID true


// ┌─────────────────────────────────────────────┐
// │                    핀 설정                    │
// └─────────────────────────────────────────────┘

// CommLink용 소프트웨어 시리얼 핀 (GND to GND)
#define COMM_RX_PIN 16  // CommLink 수신 핀 (RX) = 우노보드 TX (예: 9)
#define COMM_TX_PIN 17  // CommLink 송신 핀 (TX) = 우노보드 RX (예: 8)

// HX711 무게 센서 핀 (3.3V / GND)
#define HX_DOUT 14
#define HX_CLK 15
#define HX_FACTOR (-7050.0)     // 보정 계수

// RC522 RFID 리더기 핀 (3.3V / GND)
#define RC_SDA 5
#define RC_RST 22

// 관리자 UID 설정
constexpr const char* ADMIN_UID = "a1b2c3d4";

// 마스터 키 설정
#define MASTER_KEY "c3a27b28"

#endif