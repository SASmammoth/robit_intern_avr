# Day03_Task02

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.08.02.

---

## 1. 개요 (Overview)
본 과제는 USART를 이용하여 시리얼 통신의 기초를 배워보는 과제로써, PC의 문자열 전송을 통해 LED를 제어하고 그 반대도 시행한다.

### 핵심 목표
* 시리얼 통신을 이용해 PC에서 데이터 전송 후 LED점등
* 스위치 입력에 따라 LED 리셋하고 초기화, PC로 문자열전송

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
 PORTA (PA0 ~ PA7)       ----->      8-Bit LED
 UART1 (PD2 ~ PD3)       ----->      USART 통신
 PE4 (INT4)              ----->      SW 1 (Reset)
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **USART 통신:** UART1을 통한 시리얼 통신. 이를 위해 어제 회로에서 SW의 위치를 변경하였다. 
* **LED :** 이전회차 과제의 LED를 그대로 사용 하였다.

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day03_Task02/
│   ├── main.c
│   └── REPORT.md
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
UCSR1C = 0x06; // START 1비트/ DATA 8비트/ STOP 1비트

// RX-TX핀 입출력 설정
// D2 입력(RX), D3 출력(TX)
DDRD = 0x08;

DDRA = 0xFF; // LED 출력

EIMSK = 0x10; // INT4 사용
EICRA = 0x00; 
EICRB = 0x02; // 하강엣지 사용
sei(); // 인터럽트 실행

PORTA = ~0x00; // 모든 LED 끄기;
```

### 메인 루프
```c
recvData = Uart_Getch();	// 1byte 데이터 수신
recvData_int = recvData - '0'; // 문자를 숫자로 변환

if(recvData_int > 9) // 예외처리
{ ... }
else if (recvData_int == 8) // LED좌측으로 이동
{ ... }
else if (recvData_int == 9)// LED 우측으로 이동
{ ... }
else // 0~7의 입력은 LED 번호 바꾸고, 
{
	LED_num = recvData_int; // LED_num을 통해 출력
	...
	is_LEDon = 1; // LED 켜기
}

if (is_LEDon) // 초기화 상태인지 아닌지 확인하는 코드.
	PORTA = ~(1 << LED_num); // LED 출력
else
	PORTA = ~0x00; // LED 꺼짐

```

### 초기화 인터럽트
```c
ISR(INT4_vect)
{
	is_LEDon = 0; // 리셋
	PORTA = ~0x00; // LED 끄기
}
```


### 스위치별 동작 (ISR)

| 스위치 | 인터럽트 | 동작 패턴 |
| :---: | :---: | :--- |
| SW 0 | INT4 | 초기화 플레그를 실행하고 LED를 끔 |


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 처음 켜지면 레지스터 설정함.
2. 시리얼을 이용해 올바른 입력을 받으면 그것에 따른 LED시행
3. 0~7이 들어오면 번호에 맞는 LED 빛남
4. 8, 9가 들어오면 LED 좌, 우로 이동
5. SW 0을 누르면 초기화, LED 끄기 

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/11ejmDxygpK-3sTg_0veYayVKHGRI86Nr/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | TX 디버깅, 개념 질문 | - Uart_Pathc 함수에서 1 << TXC1 에서는 작동이 안됨. <br> - TXC1이 아니라 1 << UDRE1을 사용하는 이유 |
| **Claude** | 시리얼 줄바꿈 | - \n 에서 줄바꿈이 안되는 문제. \r\n을 사용하라 함. |



### AI 활용 및 검증 원칙
1. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화, 검색 대용)로만 활용하였습니다.
