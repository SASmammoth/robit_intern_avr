/*
 * Day04_Task02.c
 *
 * Created: 2026-08-02 오전 11:25:32
 * Author : SASmammoth
 * 
 * 날짜 프로그램 이다. 년도는 2000.01.01. 00:00:00:0000 ~ 2099.12.31. 23:23:59:9999 단위이다.
 * 따라서 윤년은 4의 배수의 날짜만이 온다.
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "liquid_crystal_i2c.h"

volatile unsigned long count_hms_ms = 0; // 시분초ms를 ms 단위로 셈
volatile unsigned long count_ymd_day = 0; // 년월일을 일 단위로 셈

volatile  uint8_t ymd_hmsms_set = 0; // 년월일 세팅할때 사용하는 변수

volatile uint8_t display_format_ymshmsms[7] = {0, }; // 년월일시분초를 저장
	
void convert_count_ymdhms(uint16_t  *display_format); // 초를 년월일_시분초로 바꾸는 함수
void set_umdhms_count(uint16_t  *display_format);
volatile uint8_t is_timer_on = 0;

int main(void)
{
	DDRA = 0xFF;
	
	TCNT0 = 0;	  // ms단위로 세기위해 prescaler을 64로 하고 CTC모드 사용.
	TCCR0 = 0x08; // CTC모드만 설정하고 타이머는 작동 X(프리스케일러를 이용해서)
	OCR0 = 249; // 1ms = 64 × x ×  62.5ns; x = 250이기 때문.
	TIMSK = 0x02;
	
	EIMSK = 0x30; // INT4, 5 사용
	EICRB = 0x0A; // INT5, INT4 하강엣지 
	
	ADMUX = 0x40; // AVCC 전압 사용 ADC0번을 단일종단이득으로 설정
	ADCSRA = 0x87; // ADC 활성화 및 분주비 128 사용
	unsigned char channel = 0x00;
	uint16_t adcValue = 0;
	
	
	i2c_master_init(100000); // i2c 통신속도 초기화
	_delay_ms(100);

	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS); // LCD 생성
	_delay_ms(50);
	lq_turnOnBacklight(&device);
	lq_clear(&device);

	lq_setCursor(&device, 0, 0);
	
	sei();
	
	uint16_t  month_day[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}}; // 윤년 테이블
	uint16_t display_format[7] = {}; // 출력할 숫자 년월일시분초ms 7개를 담고 있음.
	
	int is_leap_year = 0;
	int is_first_on = 1; // 처음 초기화 됨
	
    while (1) 
    {
		ADMUX = 0x40 | channel;
		ADCSRA |= 0x40; // 변환 시작
		while((ADCSRA&0x10) == 0); // ADC가져올때 까지 대기
		adcValue = ADC; // ADC값 저장
		ADCSRA |= 0x10;
		
		char prt_dis[2][17] = {"", ""};
			
		
		if(is_timer_on == 0) // 타이머가 동작 안 할때 가변저항을 이용하여 입력 받기
		{
			if(display_format[0] % 4 == 0) // 윤년인지 계산
				is_leap_year = 1;
			else is_leap_year = 0;
			switch(ymd_hmsms_set)
			{
				
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
				case 3:
					display_format[3] = adcValue / 44; // 시 계산
					break;
				case 4:
					display_format[4] = adcValue / 17; // 분 계산
					if(display_format[4] >= 59) display_format[4] = 59; // 59이상은 강제로 59로 변환
					break;
				case 5:
					display_format[5] = adcValue / 17; // 초 계산
					if(display_format[5] >= 59) display_format[5] = 59; // 59이상은 강제로 59로 변환
					break;
				case 6:
					display_format[6] = adcValue; // ms 계산
					if(display_format[6] >= 999) display_format[6] = 999; // 999이상은 강제로 999로 변환
					break;
			}
		}
		else if (is_first_on && is_timer_on) // 처음 타이머가 켜짐
		{
			is_first_on = 0;
			set_umdhms_count(display_format);
		}
		else
		{
			if(count_hms_ms >= 86400000) // 다음날로 변경
			{ count_hms_ms = 0; count_ymd_day++;}
			if(count_ymd_day >= 36525) // 2000~2099년의 일수
			{count_ymd_day = 0;}
			
			convert_count_ymdhms(display_format); // 일, 초를 형시에 맞게 변경
		}
		
		
		
		
		sprintf(prt_dis[0], "%2uy%2um%2ud", display_format[0], display_format[1], display_format[2]);
		sprintf(prt_dis[1], "%2uh%2um%2us%4ums", display_format[3], display_format[4], display_format[5], display_format[6]);
		
		
		lq_setCursor(&device, 0, 0);
		lq_print(&device, prt_dis[0]);
		_delay_ms(10);
		lq_setCursor(&device, 1, 0);
		lq_print(&device, prt_dis[1]);
		_delay_ms(10);
		
    }
}

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

void convert_count_ymdhms(uint16_t  *display_format)
{
	uint16_t  month_day[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
	
	uint16_t  count_leap = count_ymd_day / 1461; // 윤년의 횟수를 셈
	uint16_t  count_left = count_ymd_day % 1461; // 윤년하고 남은 일 수를 셈
	uint16_t  year_ = 0, month_ = 0, day_ = 0; // 년월일 임시저장
	uint16_t  hour_ = 0, minutes_ = 0, second_ = 0, m_second = 0;  
	uint16_t  is_leap_year = 0;
	
	if(count_left < 366) year_ = count_leap * 4;  // 윤년계산
	else year_ = count_leap * 4 + (count_left - 366) / 365 + 1; // 나머지 년도 계산
	
	uint16_t  year_form_leap = year_ - count_leap * 4; // 윤년으로 부터 연도 계산
	if (year_form_leap == 0) day_ = count_left;
	else day_ = count_left - 366 - (year_form_leap - 1) * 365; // 올해에 남은 일 수 계산
	
	if(year_ % 4 == 0)
	{
		is_leap_year = 1;
	}
	
	month_ = 1;
	while (day_ >= month_day[is_leap_year][month_])
	{
		day_ -= month_day[is_leap_year][month_]; // 일 수를 빼서 월과 일을 계산
		month_++;
	}
	day_++;
	
	hour_ = count_hms_ms / 3600000; // 시 계산
	minutes_ = (count_hms_ms / 60000) % 60; // 분 계산
	second_ = (count_hms_ms / 1000) % 60; // 초 계산
	m_second = count_hms_ms % 1000; // ms 계산 8bir로는 출력 안됨
	
	display_format[0] = year_;
	display_format[1] = month_;
	display_format[2] = day_;
	display_format[3] = hour_;
	display_format[4] = minutes_;
	display_format[5] = second_;
	display_format[6] = m_second;
}

ISR(INT4_vect)
{
	ymd_hmsms_set++;
	if(ymd_hmsms_set > 8)
		ymd_hmsms_set = 0;
}

ISR(INT5_vect)
{
	TCCR0 = 0x0C;  // 타이머 켜기
	is_timer_on = 1;
}

ISR(TIMER0_COMP_vect)
{
	count_hms_ms++;
}