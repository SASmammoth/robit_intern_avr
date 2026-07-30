/*
 * Task-03.c
 *
 * Created: 2026-07-29 오후 8:56:18
 * Author : SASmammoth
 *
 * Day_02 3번 과제
 * 2번 과제 이후에 S/W 핀 배열을 바꿈
 * 선배님이 주신 헤더파일은 핀 7개를 연결하여 사용하는 방식인데, 지금 LCD는 I2C를 이용하므로, github에서 헤더파일을 하나 들고 왔다.
 * LCD 주소는 0x27
 */ 

#define  F_CPU 16000000

#include <stdio.h> 
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "liquid_crystal_i2c.h"

volatile int16_t A_ = 0; // A값.
volatile int16_t B_ = 0; // B값.
volatile char op = '+'; // 연산자
volatile uint8_t op_i = 0; // OP인덱스.
volatile char op_list[4] = {'+', '-', '*', '/'};
volatile int16_t C_ = 0; // C값




int main(void)
{
	DDRA = 0x00;
	EIMSK = 0x5C; // 2, 3, 4, 6번 인터럽트 핀 (PD2, PD3, PE4, PE6) 사용
	
	EICRA = 0xA0; // 하강 엣지 클럭 사용
	EICRB = 0x22;
	
	sei(); // 인터럽트 사용
	
	
	i2c_master_init(100000); // i2c 통신속도 초기화
	_delay_ms(100);

	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS);
	_delay_ms(50);
	lq_turnOnBacklight(&device);
	lq_clear(&device);              

	lq_setCursor(&device, 0, 0);    
	lq_print(&device, "JKM"); // 이니셜 생성

	lq_setCursor(&device, 1, 0);    
	
	char buf_[15] = "";
	
	while (1)                       
	{
		
		
		sprintf(buf_, "%3d %c %3d = %3d", A_, op, B_, C_); // 문자열 조작을 통해 형식 설정
		
		lq_setCursor(&device, 1, 0);
		lq_print(&device, buf_); // 출력
		
		_delay_ms(100);
		
	}
}


ISR(INT2_vect) // S/W : 2
{
	B_++; // B에 1 더함
}

ISR(INT3_vect) // S/W : 3
{
	switch(op){ // switch문을 이용해 연산 수행
		case('+'):
			C_ = A_ + B_;
			break;
		case('-'):
			C_ = A_ - B_;
			break;
		case('*'):
			C_ = A_ * B_;
			break;
		case('/'):
			C_ = A_ / B_;
			break;
		default:
			break;
	}
		
}

ISR(INT4_vect) // S/W : 0
{
	A_++; // A에 1을 더함
}

ISR(INT6_vect) // S/W : 1
{
	op_i++; // 연산자 인덱스를 이용하여 + - * / 회전 실현
	if( op_i > 3)
		op_i = 0;
	op = op_list[op_i];
}