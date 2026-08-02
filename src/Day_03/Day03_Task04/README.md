# Day03_Task04

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.08.02.

---

## 1. 개요 (Overview)


### 핵심 목표
* USAT 통신의 원리를 이해하여, 레지스터를 사용하지 않고 9600bps의 USAT통신을 흉내낸다. 

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 PD3				----->			 PC
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **USART 통신:** UART을 빙자한 시리얼 통신. UART관련 레지스터를 사용하지 않고 딜레이와 입출력을 이용해 UART형식을 흉내냄. 

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day03_Task04/
│   ├── main.c
│   └── REPORT.md
└────── circuit.jpg
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 레지스터 설정 (`main.c`)
```c
DDRD = 0x08;	// PD3 출력모드로 바꿈
PORTD = 0x08;	// STOP모드로 변경
_delay_us(1000);
```

### 글자 출력 부분
```c
{// e 0110 0101 전송 | 전송 함수 등으로 작성할수도 있지만, UART통신 학습 용도로 날것으로 작성함
	PORTD = 0x00;	// start
	_delay_us(104); // 9600bps 의 폭은 약 1/9600 = 102µs 정도이다.
	PORTD = 0x08;	// 0
	_delay_us(104);
	PORTD = 0x00;	// 1
	_delay_us(104);
	PORTD = 0x08;	// 2
	_delay_us(104);
	PORTD = 0x00;	// 3
	_delay_us(104);
	PORTD = 0x00;	// 4
	_delay_us(104);
	PORTD = 0x08;	// 5
	_delay_us(104);
	PORTD = 0x08;	// 6
	_delay_us(104);
	PORTD = 0x00;	// 7
	_delay_us(104);
	PORTD = 0x08;	// End
	_delay_us(1040);
}
```


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 처음 켜지면 레지스터 설정함.
2. 글자에 맞는 비트와 딜레이를 PORT 로 출력함

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/1Bs45v1FMoo5QT2_7xMUPJFcW968S1-HI/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | TX 디버깅 | - 다른걸 전부 맞추었는데 글자가 깨지는 현상이 발생함. <br> - delay와 PORTD의 위치를 바꾸라 함. |



### AI 활용 및 검증 원칙
1. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 검색 대용)로만 활용하였습니다.
