# Day02_Task02

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.07.30.

---

## 1. 개요 (Overview)
본 과제는 ATmega128의 ADC(아날로그-디지털 변환기)를 이용해 가변저항의 아날로그 전압을 측정하고, 그 결과를 I2C 방식의 텍스트 LCD와 8-Bit LED에 동시에 출력하는 것을 목표로 한다. ADC 원시 값과 환산 전압을 LCD에 표시하고, 전압 크기에 비례하여 LED를 점등한다.

### 핵심 목표
* ADC 단일 종단(Single Ended) 변환을 통한 아날로그 전압 측정
* ADC 원시 값을 실제 전압으로 환산하여 LCD에 출력
* I2C(TWI) 통신을 이용한 텍스트 LCD 제어
* 측정 전압에 비례한 LED 표시

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, I2C 텍스트 LCD(1602), 가변저항, 8-Bit LED |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 PORTA (PA0 ~ PA7)       ----->      8-Bit LED (Active Low)
 ADC0 (PF0)              ----->      가변저항 (Analog Input)
 PD0 (SCL) / PD1 (SDA)   ----->      I2C 텍스트 LCD (주소 0x27)
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **ADC 기준 전압:** AVCC(≈5V)를 기준 전압으로 사용 (REFS = 01)
* **LCD 통신:** I2C 사용, SCL/SDA는 PD0/PD1
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

### 5.1 ADC 초기화
```c
DDRF   = 0x00;
ADMUX  = 0x40;   // AVCC 전압 사용
ADCSRA = 0x87;   // ADC 활성화 및 분주비 128 사용
```
* **ADMUX = 0x40** : 상위 REFS 비트를 `01`로 설정하여 **AVCC를 기준 전압**으로 사용한다. 하위 채널 비트는 0이므로 ADC0(PF0)을 입력으로 선택한다.
* **ADCSRA = 0x87** : ADEN(Bit 7)으로 ADC를 활성화하고, ADPS2:0 = `111`로 **분주비 128**을 설정한다. 시스템 클록 16MHz ÷ 128 = **125kHz**로, ADC 권장 범위(50~200kHz) 안에 든다.

### 5.2 ADC 변환 및 전압 환산 (메인 루프)
```c
ADMUX = 0x40 | channel;     
ADCSRA |= 0x40;                  // 변환 시작
while ((ADCSRA & 0x10) == 0);    // ADC가져올때 까지 대기
adcValue = ADC;                  // ADC값 저장

adc_Volt_100 = (unsigned long)adcValue * 500 / 1024;  // ADC값을 100배 뻥튀기 해서 소숫점 출력
```
* 변환 완료는 **ADIF 플래그를 폴링**하여 확인한다. `ADCSRA & 0x10`이 Bit 4(ADIF)를 검사한다.
* AVR-libc의 부동소수점 `printf`가 제한적이므로, 소수점 표시를 위해 **전압을 100배 정수(`adc_Volt_100`)로 계산**한 뒤 정수 나눗셈/나머지로 정수부·소수부를 나눈다. (기준 5V이므로 `adcValue × 500 / 1024`)

### 5.3 LCD 출력
```c
char buf_[15];
sprintf(buf_, "%4u %u.%2uV", adcValue, adc_Volt_100 / 100, adc_Volt_100 % 100); // 문자열 조작을 통해 전압출력
lq_setCursor(&device, 1, 0);
lq_print(&device, buf_);
```
* `adc_Volt_100 / 100`으로 정수부, `% 100`으로 소수부를 얻어 `X.XXV` 형태로 표시한다.
* LCD 초기화 시 첫 줄에 이니셜을 표시하고, 둘째 줄에 ADC 원시 값과 환산 전압을 갱신 출력한다.

### 5.4 LED 막대그래프 출력
```c
LED_num = adcValue * 8 / 1024;

LED_temp = 1;
for (int i = 0; i < LED_num; i++) {
    LED_temp <<= 1; 
    LED_temp += 1;               // 값에 따른 LED출력
}
PORTA = ~LED_temp;
```
* ADC 값을 8단계로 나누어, 전압이 클수록 더 많은 LED가 켜지는 막대그래프를 구현한다.

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 시스템 전원 인가 시 ADC 관련 레지스터(ADMUX, ADCSRA)와 I2C LCD를 초기화한다.
2. 가변저항을 돌려 ADC0(PF0)에 0~5V의 아날로그 전압을 인가한다.
3. ADC가 변환한 값을 100ms 주기로 읽어, LCD에는 원시 값과 환산 전압을, LED에는 크기에 비례한 막대그래프를 출력한다.

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/1Xb236soizmgIBrkY9Nzk8FtTH0kzqwon/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | LCD관련 문제 | - LCD 라이브러리를 적용할 때 발생하는 에러 디버깅등. |
| **Claude** | 소숫점 출력 문제 | - 소숫점 출력할 때 형식 조언 |
| **Claude** | 문서화 & 교정 | - 보고서 맞춤법 및 문서화 도움 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 제시한 레지스터 설정 및 개념은 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 보드에서 직접 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
