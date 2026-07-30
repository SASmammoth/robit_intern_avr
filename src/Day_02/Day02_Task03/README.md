# Day02_Task03

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.07.30.

---

## 1. 개요 (Overview)
본 과제는 ATmega128에서 4개의 버튼(외부 인터럽트)을 이용해 두 정수 A, B와 연산자를 입력받고, 사칙연산을 수행하여 그 결과를 I2C 텍스트 LCD에 출력하는 간이 계산기이다.

### 핵심 목표
* 외부 인터럽트(INT2, INT3, INT4, INT6)를 이용한 버튼 입력 처리
* 인터럽트에서 피연산자 증가·연산자 변경·계산 수행
* `sprintf`를 이용한 수식 문자열 구성 및 LCD 출력

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, I2C 텍스트 LCD(1602), 택트 스위치 4개 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 PD2 (INT2)              ----->      SW 2 (B 값 증가)
 PD3 (INT3)              ----->      SW 3 (계산 실행)
 PE4 (INT4)              ----->      SW 0 (A 값 증가)
 PE6 (INT6)              ----->      SW 1 (연산자 변경)
 PD0 (SCL) / PD1 (SDA)   ----->      I2C 텍스트 LCD (주소 0x27)
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **LCD 통신:** ATmega128의 TWI(하드웨어 I2C) 사용, SCL/SDA는 PD0/PD1
* **헤더 파일 교체:** 선배님이 주신 헤더는 LCD를 병렬 7핀으로 연결하는 방식이었으나, 본 과제에서는 I2C 방식 LCD를 사용하므로 GitHub의 I2C용 라이브러리(`liquid_crystal_i2c`, `i2c_master`)를 가져와 사용하였다. LCD의 I2C 주소는 0x27이다.

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day02_Task02/
│   ├── main.c                 # 메인 제어: ADC 측정, LCD/LED 출력
│   ├── i2c_master.c           # I2C(TWI) 마스터 드라이버
│   ├── liquid_crystal_i2c.c   # I2C 텍스트 LCD 드라이버
│   └── REPORT.md
├── include/
│   ├── i2c_master.h
│   └── liquid_crystal_i2c.h
└────── circuit.jpg
```
> I2C LCD 및 통신 드라이버(`i2c_master`, `liquid_crystal_i2c`)는 오픈소스 라이브러리를 활용하였다.

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 5.1 전역 변수 (인터럽트와 공유)
```c
volatile int16_t A_ = 0;                         // 피연산자 A
volatile int16_t B_ = 0;                         // 피연산자 B
volatile char    op = '+';                       // 현재 연산자
volatile uint8_t op_i = 0;                       // 연산자 인덱스
volatile char    op_list[4] = {'+','-','*','/'}; // 연산자 목록
volatile int16_t C_ = 0;                         // 계산 결과
```


### 5.2 인터럽트 초기화
```c
EIMSK = 0x5C;   // INT2, INT3, INT4, INT6 활성화
EICRA = 0xA0;   // INT2, INT3 하강 에지 트리거
EICRB = 0x22;   // INT4, INT6 하강 에지 트리거
sei();          // 전역 인터럽트 허용
```
* **EIMSK = 0x5C** : `0101 1100`이므로 Bit 2·3·4·6(INT2, INT3, INT4, INT6)을 활성화한다.
* **EICRA / EICRB** : 각 인터럽트를 하강 에지(버튼이 눌리는 순간)에서 트리거되도록 설정한다.

### 5.3 LCD 초기화 및 메인 루프
```c
i2c_master_init(100000);   // I2C 통신 속도 100kHz 설정
LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS);
lq_print(&device, "JKM");  // 첫 줄에 이니셜 표시

while (1)
{
    sprintf(buf_, "%3d %c %3d = %3d", A_, op, B_, C_); // 문자열 조작을 통해 형식 설정
	lq_setCursor(&device, 1, 0);
	lq_print(&device, buf_); // 출력
	_delay_ms(100);
}
```
* 메인 루프는 현재 A, 연산자, B, 결과 C를 `A op B = C` 형식의 문자열로 만들어 LCD 둘째 줄에 100ms 주기로 갱신 출력한다.

### 5.4 스위치별 동작 (ISR)

| 스위치 | 인터럽트 | 동작 |
| :---: | :---: | :--- |
| SW 0 | INT4 | A 값을 1 증가 |
| SW 2 | INT2 | B 값을 1 증가 |
| SW 1 | INT6 | 연산자를 `+ → - → * → /` 순서로 회전 |
| SW 3 | INT3 | 현재 연산자로 `C = A op B` 계산 실행 |

```c
ISR(INT6_vect)   // SW 1 : 연산자 변경
{
    op_i++;
    if (op_i > 3) op_i = 0;   // 인덱스를 순환시켜 4개 연산자 회전
    op = op_list[op_i];
}

ISR(INT3_vect)   // SW 3 : 계산 실행
{
    switch (op) {
        case '+': C_ = A_ + B_; break;
        case '-': C_ = A_ - B_; break;
        case '*': C_ = A_ * B_; break;
        case '/': C_ = A_ / B_; break;
        default: break;
    }
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 시스템 전원 인가 시 인터럽트 레지스터(EIMSK, EICRx)와 I2C LCD를 초기화한다.
2. SW 0(INT4), SW 2(INT2)를 누를 때마다 각각 피연산자 A, B가 1씩 증가한다.
3. SW 1(INT6)을 누르면 연산자가 `+ → - → * → /` 순으로 바뀐다.
4. SW 3(INT3)을 누르면 현재 연산자로 계산을 수행하여 결과 C를 갱신한다.
5. LCD 둘째 줄에 `A op B = C` 형태의 수식과 결과가 실시간으로 표시된다.

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/168Pao6oOGJC6SpNyHv0K1RH6A1VlhDSx/view?usp=sharing) |

> ※ 영상 링크는 Google Drive 공유 설정이 "링크가 있는 모든 사용자"로 되어 있어야 재생 가능합니다.

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | 문서화 & 교정 | - 보고서 맞춤법 및 문서화 도움 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 제시한 레지스터 설정 및 개념은 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 보드에서 직접 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
