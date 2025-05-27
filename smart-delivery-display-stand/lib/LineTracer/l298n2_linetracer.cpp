// ============================
// L298N 기반 2휠 라인트레이서
// ============================

// --- 모터 제어 핀 설정 ---
// 왼쪽 모터 제어 핀 (L298N 모터 드라이버 기준)
const int IN1 = 5;   // IN1: 왼쪽 모터 방향 제어 (정방향 or 역방향)
const int IN2 = 6;   // IN2: 왼쪽 모터 방향 제어 (IN1과 조합)

// 왼쪽 모터 속도(PWM) 제어 핀
const int ENA = 9;   // ENA: 왼쪽 모터 속도 제어 (analogWrite 사용)

// 오른쪽 모터 제어 핀
const int IN3 = 10;  // IN3: 오른쪽 모터 방향 제어
const int IN4 = 11;  // IN4: 오른쪽 모터 방향 제어
const int ENB = 3;   // ENB: 오른쪽 모터 속도 제어

// --- 라인 센서 핀 설정 ---
// 일반적으로 라인 센서는 검정색 라인을 감지하면 LOW(0), 흰색 바닥은 HIGH(1)을 반환
const int SENSOR_L = A0;  // 왼쪽 라인 센서 입력 핀
const int SENSOR_R = A5;  // 오른쪽 라인 센서 입력 핀

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작 (디버깅용)

  // 모터 제어 핀을 출력으로 설정
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // 라인 센서 핀은 입력으로 설정
  pinMode(SENSOR_L, INPUT);
  pinMode(SENSOR_R, INPUT);

  stopMotors();  // 초기 상태에서 모터 정지
}

void loop() {
  // 라인 센서로부터 현재 감지값 읽기
  int left = digitalRead(SENSOR_L);   // 왼쪽 센서 값
  int right = digitalRead(SENSOR_R);  // 오른쪽 센서 값

  // 라인 감지 결과에 따른 주행 동작 결정
  if (left == 0 && right == 0) {
    forward();         // 양쪽 모두 검정색 라인: 직진
  } else if (left == 1 && right == 0) {
    turnRight();       // 왼쪽 흰색, 오른쪽 검정: 오른쪽으로 회전
  } else if (left == 0 && right == 1) {
    turnLeft();        // 왼쪽 검정, 오른쪽 흰색: 왼쪽으로 회전
  } else {
    stopMotors();      // 둘 다 흰색: 경로 이탈 → 정지
  }
}

// ==========================
//          제어 함수
// ==========================

// 직진: 두 모터를 모두 정방향으로 회전
void forward() {
  digitalWrite(IN1, HIGH);  // 왼쪽 모터 전진
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // 오른쪽 모터 전진
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);    // PWM으로 왼쪽 모터 속도 설정
  analogWrite(ENB, 200);    // PWM으로 오른쪽 모터 속도 설정
}

// 왼쪽 회전: 오른쪽 바퀴만 회전
void turnLeft() {
  digitalWrite(IN1, LOW);   // 왼쪽 모터 정지
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // 오른쪽 모터 전진
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);      // 왼쪽 모터 속도 0
  analogWrite(ENB, 200);    // 오른쪽 모터 속도 유지
}

// 오른쪽 회전: 왼쪽 바퀴만 회전
void turnRight() {
  digitalWrite(IN1, HIGH);  // 왼쪽 모터 전진
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // 오른쪽 모터 정지
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);    // 왼쪽 모터 속도 유지
  analogWrite(ENB, 0);      // 오른쪽 모터 속도 0
}

// 정지: 모든 모터 OFF
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);  // 속도 0 → 완전 정지
  analogWrite(ENB, 0);
}