⸻

📁 프로젝트 폴더 구조 및 사용법

이 프로젝트는 PlatformIO 기반으로 작성되었으며, 폴더별 역할과 사용 방법은 아래와 같습니다:

⸻

✅ src/ - 메인 소스 코드 디렉토리
•	setup()과 loop() 함수를 포함하는 메인 실행 파일을 작성합니다.
•	모든 실행 흐름의 시작점이며, PlatformIO는 이 폴더를 자동으로 컴파일합니다.

예시:
```C++
#include <Arduino.h>
#include <MotorDriver.h>
#include "Config.h"

void setup() {
initMotor();
}

void loop() {
runMotor();
}
```

⸻

✅ lib/ - 사용자 정의 라이브러리 디렉토리
•	기능별로 클래스를 모듈화하여 폴더 단위로 분리합니다.
•	각 하위 폴더는 하나의 독립적인 라이브러리로 인식되며 자동으로 컴파일됩니다.

예시 구조:
```C++
lib/
└── MotorDriver/
├── MotorDriver.h
└── MotorDriver.cpp
```

사용법:
```
#include <MotorDriver.h>  // <> 사용
```

⸻

✅ include/ - 전역 헤더 디렉토리
•	전역 상수, 매크로, 공통 구조체, 설정 값 등을 정의합니다.
•	여러 소스 파일에서 공유되는 선언을 한 곳에 모아둘 수 있습니다.

예시:
```C++
include/
└── Config.h
```
```C++
// Config.h
#define MOTOR_PIN 13
```
사용법:
```C++
#include "Config.h"  // "" 사용
```

⸻

✅ test/ - 테스트 코드 디렉토리 (선택적)
•	Unity 프레임워크 기반의 유닛 테스트 코드를 작성합니다.
•	pio test 명령어로 자동 실행됩니다.

예시:
```C++
test/
└── test_motor.cpp
```

```
#include <Arduino.h>
#include <unity.h>

void test_motor_init() {
TEST_ASSERT_EQUAL(1, 1);  // 간단한 테스트
}

void setup() {
UNITY_BEGIN();
RUN_TEST(test_motor_init);
UNITY_END();
}

void loop() {}
```

⸻

✅ 기타
•	.pio/: PlatformIO 빌드 캐시 (자동 생성, Git에 포함하지 않음)
•	.gitignore: 자동 생성 파일, 바이너리 등을 Git에서 제외
•	platformio.ini: 전체 프로젝트 설정 파일 (보드, 포트, 라이브러리 등)

⸻

📌 참고
•	라이브러리 생성 가이드: https://docs.platformio.org/page/librarymanager/creating.html
•	플랫폼 구조 가이드: https://docs.platformio.org/page/projectconf/structure.html

⸻