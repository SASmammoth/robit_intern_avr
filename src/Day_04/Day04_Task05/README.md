# Day04_Task05

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.08.02.

---

## 1. 개요 (Overview)
PWM파형을 생성하여 서보모터를 동작한다.

### 핵심 목표
* PWM 제어의 원리를 파악하여 서보모터의 각도 제어
* 시리얼 통신의 값을 가져와 각도 입력

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, 서보모터 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 PORTA (PA0 ~ PA7)       ----->      8-Bit LED
 UART1 (PD0:1)			 ----->      USART 통신
 PB7 		             ----->      PWM서보모터
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **TIMER0/2:** TIMER0/2를 이용하여 PB7에 연결되어 있는 서보모터에 PWM 신호 전송
* **서보모터 :** 서보모토가 각도에 따라 돌아감
---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day04_Task05/
│   ├── main.c                 
│   └── README.md
└────── circuit.jpg
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 레지스터 설정 (`main.c`)
```c
DDRB = 0x80; // 서보모터 PWM

TCCR2 = 0x0A; // 프리스케일러 8, count 11, CTC모드를 이용하여 5.55마이크로초 만큼 카운트 함 : 서보모터 1degree
OCR2 = 10;
TCCR0 = 0x0C; // 프리스케일러 64, count 250, CTC모드를 이용하여 1ms 만큼 카운트 함
OCR0 = 249;
TIMSK = 0x82;

UBRR1H = 0x00; // PC와 시리얼 통신
UBRR1L = 103; // 9600bps 사용
UCSR1A = 0x20; // 송신, 수신 상태비트 초기화
UCSR1B = 0x18; // 송신, 수신 기능 활성화
UCSR1C = 0x86;
```

### PWM 생성
```c
ISR(TIMER0_COMP_vect) 
{
	count20ms++;
	if(count20ms > 20)
	{
		PORTB = 0x80; // PWM시작 (20초 마다 초기화)
		count20ms = 0;
		count180deg = 0;
	}
}

ISR(TIMER2_COMP_vect) 
{ 
	count180deg++;
	if(count180deg >= 91 + prev_angle * 2) // 0.5ms 기본 = 91 각도에 따른 폭 조절
		PORTB = 0x00;
}
...
```

### 시리얼 통신을 통한 각도 입력 받기
```c
uint8_t get_angle( uint8_t prev_angle)
{
	char temp_[4]; // 임시 버퍼
	...
	int is_error = 1;
	for(int i = 0; i < 3; i++)
	{
		temp_[i] = Uart_Getch(); // 문자 하나 입력
		Uart_Putch(temp_[i]);
		
		if(!(temp_[i] >= '0' && temp_[i] <= '9')) // 이상한 값이 들어오면 error반환
			is_error = 0; 
	}
	
	temp_[3] = '\0';
	
	int temp_int = atoi(temp_); // 출력값 생성
	if(!is_error)
	{
		print_serial("error");
		return prev_angle;
	}
	else if(temp_int < 0 || 180 < temp_int) // 예외처리
	{
		print_serial("error");
		return prev_angle;
	}
	...
	return temp_int;
}
```


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 초기화 시 기본 각 90으로 이동
2. 시리얼 통신으로 각도 입력(0~180도 입력처리는 하였지만, 입력이 3자리가 안 들어오면 작동 안함.)
3. 각도에 따른 PWM을 이용해 서보모터 동작

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/1lda2e9j8I4t8Z2GTZNXEHhVXhbRGBf4u/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | PWM 타이머 계산 | - 8bit 타이머를 최대로 해도 20ms가 안되는 문제 <br> - count++를 이용해서 S/W적으로 처리하라 함. |
| **Claude** | 코드 디버깅 | - interrupt.h, sei() 누락 <br> - 시리얼 입력 때문에 PWM이 동작하지 않는 부분을 인터럽트로 옮기라 조언 |
| **Claude** | 서보모터 PWM 디버깅 | - 인터넷에 나와있는 서보모터의 PWM으로 계산하니까 서보모터 회전이 절반으로 축소되는 문제 ex) 180입력 -> 90출력  <br> - 펄스폭이 다를수 있고 다른 폭들을 알려줌 |



### AI 활용 및 검증 원칙
1. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화, 검색 대용)로만 활용하였습니다.
