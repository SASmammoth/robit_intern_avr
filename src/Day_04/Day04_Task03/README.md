# Day04_Task03

> **광운대학교 컴퓨터정보공학부**<br>
> **작성자:** 장경민<br>
> **제출일:** 2026.08.02.<br>

---

## 1. 개요 (Overview)
PSD 센서를 이용해 거리를 측정하는 프로그램이다.

### 핵심 목표
* 2Y0A02 PSD 센서를 ADC를 이용해서 입력 받음.
* ADC값을 적절한 식을 이용해 거리를 구한다.

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, PSD |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 USART1 (PD0:1)			 ----->		 PC 시리얼 통신
 ADC7 (PF7)              ----->      PSD 
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **PSD 센서:** PSD센서를 이용해 거리를 측정한다. 

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day04_Task03/
│   ├── main.c
└────── circuit.jpg
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 레지스터 설정 (`main.c`)
```c
UBRR1H = 0x00;
UBRR1L = 103; // 9600bps 사용
UCSR1A = 0x20; // 송신, 수신 상태비트 초기화
UCSR1B = 0x18; // 송신, 수신 기능 활성화
UCSR1C = 0x86;

ADMUX = 0x47; // AVCC 전압 사용 ADC7번을 단일종단이득으로 설정
ADCSRA = 0x87; // ADC 활성화 및 분주비 128 사용
```

### 거리 값 구하기
```c
...
adcValue = ADC; // ADC값 저장
ADCSRA |= 0x10;

voltage = adcValue * (5.0/1024.0); // 전압을 먼저 구함

char prt_chr[25] = ""; // 출력 문자열
length = 60.495 * pow(voltage, -1.1904); // 인터넷에서 가져온 가장 많이쓰인다는 가장 많이 사용되는 근사공식
if(voltage > 2.5 || voltage < 0.4) // 예외처리 (20cm이하, 1.5m이상은 error이다.)
	sprintf(prt_chr, "error");
else
	sprintf(prt_chr, "%4dADC %3dcm\r\n",adcValue, (int) length);
```


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 전압이 0.4v ~ 2.5v(150cm ~ 20cm)이면 ADC값과 거리를 출력한다.
2. 범위 밖이면 "error"를 출력한다.

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/150YwfKhFANgqoS6k-gZ8oI9MzzYaY-Z-/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | 센서 이론 | - 거리 계산식 관련 자료를 찾음. |
| **Claude** | 회로 디버깅 | - ADC값이 붕 뜨는문제 <br> - 공통접지, 풀업저항 불필요, 센서의 전압 특성(0.4v ~ 2.6v)등을 알게되었다. |




### AI 활용 및 검증 원칙
1. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화, 검색 대용)로만 활용하였습니다.
