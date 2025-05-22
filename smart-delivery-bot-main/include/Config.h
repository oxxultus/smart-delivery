/*
* 해당 파일은 전역적으로 사용하는 프로그램 설정 파일 입니다.
* 공통적으로 사용하는 설정들을 해당 파일에서 관리 합니다.
*/
#ifndef CONFIG_H
#define CONFIG_H


// ┌─────────────────────────────────────────────┐
// │                   기기 설정                   │
// └─────────────────────────────────────────────┘

// WiFi 정보
#define WIFI_SSID     "YourSSID"
#define WIFI_PASSWORD "YourPassword"

// 메인 서버 IP 및 포트
#define MAIN_SERVER_IP   "192.168.0.100"
#define MAIN_SERVER_PORT 8080

// 내부 서버 포트 설정
#define INNER_SERVER_PORT 8081

// 메인 서버 API 요청 URL
#define GET_PAYMENT "/bot/payment"

// 시리얼 속도 설정
#define SERIAL_BAUDRATE 115200

// 관리자 UID 설정
constexpr const char* ADMIN_UID = "a1b2c3d4";


// ┌─────────────────────────────────────────────┐
// │                    핀 설정                    │
// └─────────────────────────────────────────────┘

// CommLink용 소프트웨어 시리얼 핀 (GND to GND)
#define COMM_RX_PIN D8  // CommLink 수신 핀 (RX) = 우노보드 TX (예: 9)
#define COMM_TX_PIN D0  // CommLink 송신 핀 (TX) = 우노보드 RX (예: 8)


// HX711 무게 센서 핀 (3.3V / GND)
#define HX_DOUT D1
#define HX_CLK D2
#define HX_FACTOR (-7050.0)     // 보정 계수

// RC522 RFID 리더기 핀 (3.3V / GND)
#define RC_SDA D4
#define RC_RST D3
/* TODO: 연결 정보
* #1 SDA (SS/CS)  D4 (GPIO2)
* #2 SCK          D5 (GPIO14)
* #3 MOSI         D7 (GPIO13)
* #4 MISO         D6 (GPIO12)
* #5 RST          D3 (GPIO0)
*/


#endif