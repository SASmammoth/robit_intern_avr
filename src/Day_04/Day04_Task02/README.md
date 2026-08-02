# Day04_Task02

> **광운대학교 컴퓨터정보공학부**  
> **작성자:** 장경민  
> **제출일:** 2026.08.02.

---

## 1. 개요 (Overview)
TIMER을 통해 시간경과를 인식하여 달력프로그램의 날짜를 바꾸는 프로그램이다.

### 핵심 목표
* 가변저항과 SW0를 이용해서 초기 시간 세팅
* SW1를 이용해서 시간경과 확인, LCD를 이용해서 전시

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, LCD, 택트 스위치 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                         [Target Component]
 SDA/SCL (PD0:1)		 ----->      LCD I2C통신
 PE4 (INT4)              ----->      SW0 (setting)
 PE5 (INT5)              ----->      SW1 (start)
 ADC0 (PF0)              ----->      가변저항
```

### 주요 회로 특징
* **전원:** USB를 이용한 전원 공급
* **TIMER0:** TIMER0를 이용하여 ms단위의 시간 타이머 사용함.
* **LCD와 SW :** 달력을 LCD로 전시하고, SW를 이용해 제어한다.
* **가변저항 :** 현재 SW를 바탕으로 시간을 설정할 때 사용한다.

---

## 4. 프로젝트 구조 (Directory Structure)
```text
├── Day04_Task02/
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
TCNT0 = 0;	  // ms단위로 세기위해 prescaler을 64로 하고 CTC모드 사용.
TCCR0 = 0x08; // CTC모드만 설정하고 타이머는 작동 X(프리스케일러를 이용해서)
OCR0 = 249; // 1ms = 64 × x ×  62.5ns; x = 250이기 때문.
TIMSK = 0x02;

EIMSK = 0x30; // INT4, 5 사용
EICRB = 0x0A; // INT5, INT4 하강엣지 

ADMUX = 0x40; // AVCC 전압 사용 ADC0번을 단일종단이득으로 설정
ADCSRA = 0x87; // ADC 활성화 및 분주비 128 사용
```

### 타이머 세팅
```c
case 0:
	display_format[0] = adcValue / 10; // adc의 최대값은 1023이므로 적절한 수로 나누어 계산
	if(display_format[0] >= 100) display_format[0] = 99; // 99이상은 강제로 99로 변환
	break;
case 1:
	display_format[1] = adcValue / 93 + 1; // 월 계산
	break;
case 2:
	display_format[2] = adcValue / 34 + 1;
	if(display_format[2] >= month_day[is_leap_year][display_format[1]]) // 윤년과 월일표를 이용하여 맞는 최애 날짜 구함.
		display_format[2] = month_day[is_leap_year][display_format[1]];
	break;
...
```

### 입력 받은 날짜/시간을 ms, d 단위로 변환
```c
void set_umdhms_count(uint16_t  *display_format)
{
	uint16_t  month_day[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
	
	uint16_t year_ = display_format[0]; // 년도
	uint16_t month_ = display_format[1]; // 월
	uint16_t day_ = display_format[2]; // 일
	
	
	count_ymd_day = (year_/4) * 1461; // 직전 윤년까지 더함
	
	uint8_t left_year = year_ % 4;
	if(left_year > 0) count_ymd_day += 366 + (left_year - 1) * 365; // 직전 년도 까지 날짜 계산
	
	int is_leap_year = 0;
	
	if (year_ % 4 == 0)
		is_leap_year = 1;
	
	for(int i = 0; i < month_; i++)
		count_ymd_day += month_day[is_leap_year][i]; // 직전 월 까지 날짜 더함
	
	count_ymd_day += day_ - 1; // 날짜 더함
	
	count_hms_ms = (unsigned long)display_format[3] * 3600000 + (unsigned long)display_format[4] * 60000 + (unsigned long)display_format[5] * 1000 +  (unsigned long)display_format[6];
}
```

### 타이머 시작
```c
ISR(INT5_vect)
{
	TCCR0 = 0x0C;  // 타이머 켜기
	is_timer_on = 1;
}
```

### 스위치별 동작 (ISR)

| 스위치 | 인터럽트 | 동작 패턴 |
| :---: | :---: | :--- |
| SW 0 | INT4 | 년-월-일-시-분-초-ms의 설정을 바꿈 |
| SW 1 | INT5 | 시간흐름의 시작과 종료를 선택함 |


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. SW0과 가변저항을 이용해 초기 시각을 입력함
2. SW1을 누르면 시간이 흐름

### 동작 사진 / 영상

| 동작 모습 |
| :---: |
| [▶ 동작 영상 보기](https://drive.google.com/file/d/14U49QUBcBQizH4l_BZ-2y1u15jshKIf5/view?usp=sharing) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)는 **Claude AI**이며, 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | 윤년 계산 | - 윤년 계산에 대한 에러 수정 |
| **Claude** | 타이머 디버깅 | - 프리스케일러 설정오류 |



### AI 활용 및 검증 원칙
1. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화, 검색 대용)로만 활용하였습니다.
