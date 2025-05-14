/*
* 해당 파일은 전역적으로 사용하는 프로그램 설정 파일 입니다.
* 공통적으로 사용하는 설정들을 해당 파일에서 관리 합니다.
*/

#ifndef CONFIG_H
#define CONFIG_H

// WiFi 정보
#define WIFI_SSID     "YourSSID"
#define WIFI_PASSWORD "YourPassword"

// 서버 IP 및 포트
#define SERVER_IP   "192.168.0.100"
#define SERVER_PORT 8080

// 요청 기본 경로
#define GET_PATH "/command"
#define POST_PATH "/receive"

#endif