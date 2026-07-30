# Day01_Task02

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민
> 
> **제출일:** 2026.07.30.

---

## 1. 개요 (Overview)
본 과제는 ATmega128 마이크로컨트롤러에서 처음으로 만드는 과제로 스위치를 이용해 LED를 조작하고 인터럽트를 수행한다.

### 핵심 목표
* ATmega128 레지스터 설정을 통한 LED 동작
* 스위치 회로 조립 및 스위치 입력
* 버튼을 이용한 및 인터럽트 제어

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, DC/STEP 모터, ADC 센서 모듈 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 PORTA (PA0 ~ PA7)       ----->      8-Bit LED
 PORTC Pin 2 (PB 0:1)    ----->      SW 1, 2 input
 PORTD Pin 2 (PB 2:3)    ----->      Interrupt SW
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day01_Task02/
    ├── main.c # ADC 데이터 수신 드라이버
    └── REOPRT.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 스위치입력의 마스크
```c
#include <avr/io.h>
#include <avr/interrupt.h>

    unsigned char SW_C = 0, SW_D = 0; // 스위치가 풀업 방식이므로 입력 레지스터 사용을 용의하게 하기위해 사용
	unsigned char MS_c = 0x03; // 0000 0011을 의미한다.
	unsigned char MS_d = 0x0C; // 0000 1100을 의미한다.

    while (1)
	{
        SW_C = PINC ^ MS_c; // 마스크와 베타적 논리 합을 이용하여 깔끔하게 스위치 입력 확인
		SW_D = PINC ^ MS_d;
        ...
    }
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 시스템 전원 인가 시 ATmega128 입출력 관련 레지스터 설정 함(DDRx, EIMSK, EICRx)
2. 기본으로 모든 LED 반짝 거림
3. 스위치 입력에 맞는 동작 시행

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/15c8jPi8f2ZNYFTp55i4Kv7ADZIRoqqlf/view?usp=sharing) |
| 과제에서는 인터럽트에서 LED가 한번만 이동해야 함. 제출할 때 영상을 보니 왕복하게끔 코드를 짠 것을 알게됨. <br> 이미 회로를 수정한 뒤라 정정을 할 수 없음. 따라서 어쩔수 없이 과제와 약간 다르지만 조금 더 나아간 코드/영상을 첨부함. |


---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI) **ClaudeAI** 이고 사용 현황 및 목적은 다음과 같음.
| 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- |
| 없음 | 없음 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.