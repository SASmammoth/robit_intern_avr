# Day01_Task03

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.07.30.

---

## 1. 개요 (Overview)
본 과제는 Day01_Task02의 스위치·인터럽트 과제를 확장한 것으로, 4개의 외부 인터럽트 스위치를 이용해 8-Bit LED를 다양한 패턴으로 제어하는 것을 목표로 한다. 각 스위치를 누르면 서로 다른 LED 이동 패턴이 인터럽트로 실행되며, 평상시에는 2진 카운터 값이 LED에 표시된다.

### 핵심 목표
* 외부 인터럽트(INT2, INT3, INT4, INT6) 설정 및 다중 인터럽트 처리
* 각 스위치별 서로 다른 LED 패턴을 인터럽트를 이용하여 구축

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, 8-Bit LED, 택트 스위치 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 PORTA (PA0 ~ PA7)       ----->      8-Bit LED (Active Low)
 PD2 (INT2)              ----->      SW 2
 PD3 (INT3)              ----->      SW 3
 PE4 (INT4)              ----->      SW 0
 PE6 (INT6)              ----->      SW 1
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **인터럽트 핀 변경:** 원래 회로 파일에서는 INT 번호가 지정되어 있었으나, LCD에서 I2C를 사용하면서 기존 INT 핀과 충돌이 발생하였다. 따라서 INT 번호를 임의로 재배치하여 사용하였다.

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day01_Task03/
│   ├── main.c
│   └── REPORT.md
└────── circuit.jpg
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 인터럽트 초기화 (`main.c`)
```c
EIMSK = 0x5C; // 2, 3, 4, 6번 인터럽트 핀 (PD2, PD3, PE4, PE6) 사용
EICRA = 0xA0; // 하강 엣지 클럭 사용
EICRB = 0x22;

sei();         // 전역 인터럽트 허용
```

* **EIMSK = 0x5C** : `0101 1100`이므로 Bit 2·3·4·6(INT2, INT3, INT4, INT6)을 1로 설정하여 해당 인터럽트를 활성화한다.

### 메인 루프
```c
volatile uint8_t counter = 0; // 인터럽트와 공유하므로 전역 변수로 선언

while (1)
{
    PORTA = ~counter;   // Active Low: counter 값을 LED에 표시
    _delay_ms(100);
    counter++;          // 평상시에는 카운터가 증가하며 LED가 변화
}
```
평상시에는 `counter`가 100ms마다 1씩 증가하며 그 값이 LED에 반전 출력된다. 스위치가 눌리면 해당 인터럽트가 발생하여 각기 다른 패턴이 실행된다.

### 스위치별 동작 (ISR)

| 스위치 | 인터럽트 | 동작 패턴 |
| :---: | :---: | :--- |
| SW 0 | INT4 | LED 3칸을 왼쪽으로 이동시킨다 |
| SW 1 | INT6 | LED 3칸을 오른쪽으로 이동시킨다 |
| SW 2 | INT2 | LED가 좌측 이동 후 우측이동 |
| SW 3 | INT3 | `counter`를 0으로 초기화 |

```c
ISR(INT3_vect)   // SW 3 : 카운터 초기화
{
    counter = 0;
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 시스템 전원 인가 시 입출력 관련 레지스터(DDRx, EIMSK, EICRx)를 설정한다.
2. 평상시에는 `counter`가 증가하며 그 값이 8-Bit LED에 표시된다.(이진카운터)
3. 각 스위치(SW 0~3) 입력 시 대응하는 인터럽트가 발생하여 서로 다른 LED 패턴을 실행한다.

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/1kjaLOgcfiRi4sY4XVGOtleKsgsSIf0-j/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | 문서화 & 개념 확인 | - 보고서 맞춤법 및 형식 교정 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 제시한 레지스터 설정 및 개념은 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 보드에서 직접 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
