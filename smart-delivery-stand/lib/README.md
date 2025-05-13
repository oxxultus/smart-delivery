⸻

📁 이 디렉터리(lib/)는 프로젝트 전용(비공개) 라이브러리를 위한 용도입니다.

PlatformIO는 이 폴더에 있는 라이브러리들을 **정적 라이브러리(static library)**로 컴파일한 뒤,
최종 실행 파일에 자동으로 링크합니다.

⸻

📌 라이브러리 작성 방식

각 라이브러리의 소스 코드는 별도의 폴더에 넣어야 합니다.
예: lib/your_library_name/[코드 파일들]

⸻

✅ 예시: Foo와 Bar 라이브러리 구조

|-- lib
|  |
|  |-- Bar
|  |  |-- docs
|  |  |-- examples
|  |  |-- src
|  |     |- Bar.c
|  |     |- Bar.h
|  |  |- library.json (선택 사항: 빌드 옵션 지정 등)
|  |
|  |-- Foo
|  |  |- Foo.c
|  |  |- Foo.h
|  |
|  |- README --> 이 파일
|
|- platformio.ini
|-- src
   |- main.c


⸻

✅ 예시: src/main.c에서 Foo와 Bar 사용

#include <Foo.h>
#include <Bar.h>

int main (void)
{
  ...
}


⸻

🧠 참고: PlatformIO의 라이브러리 의존성 탐지기(LDF)

PlatformIO는 소스 코드 내 #include 구문을 자동으로 분석하여,
필요한 라이브러리를 자동으로 찾아서 빌드에 포함합니다.

자세한 설명은 아래 공식 문서를 참고하세요:
🔗 https://docs.platformio.org/page/librarymanager/ldf.html

⸻