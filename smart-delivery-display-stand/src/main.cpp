#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Stepper.h>

// ESP-01 핀
#define ESP_RX 2
#define ESP_TX 3
SoftwareSerial esp(ESP_RX, ESP_TX);

// WiFi 정보
const char* ssid = "iptime";
const char* password = "";
const char* host = "oxxultus.kro.kr";
const int port = 8080;
const String uid = "c3a4e195";

// Stepper 모터 핀
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void waitForBoot() {
  Serial.println("ESP-01 부팅 대기 중...");
  unsigned long timeout = millis() + 10000;
  String buffer = "";
  while (millis() < timeout) {
    while (esp.available()) {
      char c = esp.read();
      buffer += c;
      Serial.print(c);
      if (buffer.indexOf("ready") != -1) {
        Serial.println("✅ ESP Ready!");
        return;
      }
    }
  }
  Serial.println("❌ ESP 부팅 실패 - 'ready' 수신 못함");
}

void sendAT(String cmd, unsigned long waitTime = 2000) {
  esp.println(cmd);
  delay(waitTime);
  while (esp.available()) {
    Serial.write(esp.read());
  }
}

String sendATWithResponse(String cmd, unsigned long waitTime = 3000) {
  esp.println(cmd);
  String response = "";
  unsigned long start = millis();
  while (millis() - start < waitTime) {
    while (esp.available()) {
      response += char(esp.read());
    }
  }
  return response;
}

bool connectAndSendRequest(String url, String& responseOut) {
  String request = "GET " + url + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

  String connectCmd = "AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(port);
  String connectResp = sendATWithResponse(connectCmd, 5000);
  if (connectResp.indexOf("OK") == -1 && connectResp.indexOf("ALREADY CONNECTED") == -1) {
    Serial.println("❌ 서버 연결 실패");
    return false;
  }

  esp.println("AT+CIPSEND=" + String(request.length()));
  unsigned long t = millis();
  bool promptFound = false;
  while (millis() - t < 3000) {
    if (esp.find(">")) {
      promptFound = true;
      break;
    }
  }

  if (!promptFound) {
    Serial.println("❌ '>' 수신 실패");
    return false;
  }

  esp.print(request);
  String response = "";
  t = millis();
  while (millis() - t < 5000) {
    while (esp.available()) {
      response += char(esp.read());
    }
  }

  responseOut = response;
  return true;
}

void setup() {
  Serial.begin(9600);
  esp.begin(9600);
  myStepper.setSpeed(10);
  delay(1000);

  Serial.println("ESP-01 초기화...");
  sendAT("AT");
  sendAT("AT+RST", 3000);
  waitForBoot();
  sendAT("AT+CWMODE=1");

  // ✅ WiFi 연결될 때까지 재시도
  while (true) {
    sendAT("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"", 8000);
    String ipResp = sendATWithResponse("AT+CIFSR", 3000);
    if (ipResp.indexOf("STAIP") != -1 || ipResp.indexOf("192.") != -1) {
      Serial.println("✅ WiFi 연결 성공");
      break;
    }
    Serial.println("❌ 연결 실패, 재시도 중...");
    delay(3000);
  }
}

void loop() {
  // [1] 작업 확인
  Serial.println("[STEP 1] /check/working-list");
  String checkUrl = "/check/working-list?uid=" + uid;
  String response;
  if (!connectAndSendRequest(checkUrl, response)) {
    delay(5000);
    return;
  }

  // [2] 응답에서 count 파싱
  int jsonStart = response.indexOf('{');
  int jsonEnd = response.lastIndexOf('}');
  if (jsonStart != -1 && jsonEnd != -1) {
    String body = response.substring(jsonStart, jsonEnd + 1);
    int colon = body.indexOf(":");
    int end = body.indexOf("}", colon);
    if (colon != -1 && end != -1) {
      int count = body.substring(colon + 1, end).toInt();
      Serial.print("▶ 작업 개수: ");
      Serial.println(count);

      // [3] 모터 동작
      for (int i = 0; i < count; i++) {
        Serial.print("🌀 ");
        Serial.println(i + 1);
        myStepper.step(stepsPerRevolution);
        delay(1000);
      }

      // [4] 작업 완료 요청
      Serial.println("[STEP 2] /end/working-list");
      String dummy;
      connectAndSendRequest("/end/working-list?uid=" + uid, dummy);
      Serial.println("✅ 완료 요청 전송");
    } else {
      Serial.println("⚠️ count 파싱 실패");
    }
  } else {
    Serial.println("⚠️ JSON 응답 파싱 실패");
  }

  delay(15000);  // 다음 루프까지 대기
}