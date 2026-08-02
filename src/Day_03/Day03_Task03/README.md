# Day03_Task03

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.08.02.

---

## 1. 개요 (Overview)
본 과제는 UART와 MAX486을 이용하여 가변저항의 값을 이용해 다이나믹셀을 제어하고 속도와 위치를 LCD에 표시한다.

### 핵심 목표
* 시리얼 통신을 이용해 PC에서 다이나믹셀의 속도를 설정함
* 가변저항의 값을 이용해 다이나믹셀의 목표 위치를 설정함
* 목표속도와 목표위치를 LCD에 출력함

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, MX-64 다이나믹셀, LCD, 가변저항 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]							[Target Component]
 UART1 (PD2 ~ PD3)		----->       USART 통신(PC)
 ADC0 (PF0)				----->		 가변 저항
 R/TXD0 (PE0 ~ PE1)		----->		 485 통신 데이터
 PE2					----->		 485 통신 송수신 모드
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **USART 통신:** UART1을 통한 PC와의 시리얼 통신
* **MAX485 :** UART0을 이용하여 MX-64 다이나믹셀에 명령을 내림

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day03_Task03/
│   ├── main.c                 
│   ├── i2c_master.c          
│   ├── liquid_crystal_i2c.c   
│   └── REPORT.md
├── include/
│   ├── i2c_master.h
│   └── liquid_crystal_i2c.h
└────── circuit.jpg
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 레지스터 설정 (`main.c`)
```c
UBRR1H = 0x00; // PC와의 통신
UBRR1L = 103; // 9600bps 사용 
UCSR1A = 0x20; // 송신, 수신 상태비트 초기화
UCSR1B = 0x18; // 송신, 수신 기능 활성
UCSR1C = 0x06; // START 1비트/ DATA 8비트/ STOP 1비트
DDRD = 0x08; // RX-TX핀 입출력 설정 | D2 입력(RX), D3 출력(TX)
UCSR1B |= 0x80; // 수신완료 인터럽트 활성화 
	

UBRR0H = 0x00; // 다이나믹셀 과의 통신
UBRR0L = 34; // 16MHz, U2X=1 → 57600bps, 오차 0.79% LLM 사용
UCSR0A = 0x22; // 송신, 수신 상태비트 초기화
UCSR0B = 0x18; // 송신, 수신 기능 활성
UCSR0C = 0x06; // START 1비트/ DATA 8비트/ STOP 1비트
DDRE = 0x01; // RX-TX핀 입출력 설정 | E0 입력(RX), E1 출력(TX)

DDRA = 0xFF;

EIMSK = 0x10; // INT4 사용
EICRA = 0x00;
EICRB = 0x02; // 하강엣지 사용

DDRE |= 0x04; // PE2 MAX485 송수신 전환으로 설정

ORTA = ~0x00; // 모든 LED 끄기;
	
ADMUX = 0x40; // AVCC 전압 사용 ADC0번을 단일종단이득으로 설정
ADCSRA = 0x87; // ADC 활성화 및 분주비 128 사용
```

### 메인 루프
```c
... // ADC변환
adcValue = ADC; // ADC값 저장
ADCSRA |= 0x10;

if(speed_update_flag)
{
	set_moter_speed(1, moter_speed_input); // 플레그를 받을때만 실행
	speed_update_flag = 0;
}

char buf_[16];
char num_[8];

... // LCD 출력

Set_Goal_Position(1, adcValue); // 모터 돌리기
_delay_ms(1000); // 딜레이를 줄이면 명령이 씹히는 문제가 발생함

```

### 다이나믹셀 입력
```c
void Putch_Dcell(uint8_t *PutData, uint8_t size)
{ // 다이나믹셀에 데이터 넣기
	
	// 송신 (에코 방지 포함) LLM의 추천
	PORTE = 0x04;
	UCSR0B &= ~(1 << RXEN0);
	UCSR0A |= (1 << TXC0);
		
	for(uint8_t i = 0; i < size; i++)
	{
		while(!(UCSR0A & (1 << UDRE0))); // 입력이 끝날때 까지 대기
		UDR0 = PutData[i]; // 다음 데이터 넣기
	}
	
	uint16_t t = 0; // LLM의 수정 : 모든 바이트가 전송될 때 까지 대기
	while(!(UCSR0A & (1 << TXC0)))
	if(++t > 60000) break;
	
	PORTE = 0x00; // 에코 방지 해제
	UCSR0B |= (1 << RXEN0); // RX켜기
}
```


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 처음 켜지면 레지스터 설정함.
2. ADC값을 구하여 그 크기 만큼 모터를 돌린다.
3. 만약 시리얼 입력이 들어오면 이를 이용해 모터의 회전 속도를 설정한다.
4. 위 두 값을 LCD에 출력한다.

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/1v7KQNmuSI3xC3NzqDDf2ljKd5zAKwyo3/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | RX-485 통신 디버깅 | - baud rate 계산 오류(U2X, UBBR등)  |
| **Claude** | 코드 디버깅 | - 수신 무한대기를 해결함 |
| **Claude** | 다이나믹셀 학습 | - Protocol 2.0 패킷 구조(헤더/ID/Length/INST/Parameter/CRC) 및 CRC-16 원리 학습 <br> - 패킷을 이용하여 write명령을 하면 컨트롤 테이블 데이터가 수정되어 원하는 명령을 내릴수 있다는 것을 알게됨 |
| **Claude** | LCD 디버깅 | - LCD에서 글자가 깨지는 문제를 발견 <br> - itoa, strcat(buf_, "    "), 인터럽트 시간 최소화 등으로 해결함 |




### AI 활용 및 검증 원칙
1. **학습 주도성:** 이번 과제에서 다이나믹셀 공부겸 Ping 수신을 시도하였으나 제대로 되지 않음. 따라서 모터의 상태가 괜찮은지 알기위해 모터를 돌리는 함수를 작성해 달라고 하였음. 이때 모터가 잘 동작함. 만들어 준 코드를 지우기도 뭐 하니, 모터의 각도 함수는 그대로 두고 관련 코드를 학습하여 다이나믹셀 데이터 시트를 보고 모터의 속도를 지정하는 함수를 직접 제작함.
