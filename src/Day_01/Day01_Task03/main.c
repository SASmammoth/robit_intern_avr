/*
 * Tesk-03.c
 *
 * Created: 2026-07-29 오후 8:07:55
 * Author : SASmammoth
 *
 * 1일차 과제 3번.
 * 2번 과제 이후에 S/W 핀 배열을 바꿈
 * 과제에서는 INT번호가 지정되어 있지만, LCD에서 I2C를 사용하면서 INT핀이 어그러 지게 됨. 따라서 임의로 INT번호를 바꿈.
 */ 

#define  F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t counter = 0; // 인터럽트와 변수를 공유하기 위함.



int main(void)
{
    /* Replace with your application code */
	DDRA = 0xFF; // 핀 입출력 설정
	DDRC = 0x00;
	DDRD = 0x00;
	
	EIMSK = 0x5C; // 2, 3, 4, 6번 인터럽트 핀 (PD2, PD3, PE4, PE6) 사용
	
	EICRA = 0xA0; // 하강 엣지 클럭 사용
	EICRB = 0x22;
	
	
	
	
	sei();
	
	
	while (1)
	{
		PORTA = ~counter; // 이진카운터를 계속 출력
		_delay_ms(100);
		counter++;
	}
}

ISR(INT2_vect) // S/W : 2
{
	counter = 0x80;
	for(uint8_t i = 0; i < 8; i++)
	{
		PORTA = ~counter;
		_delay_ms(500);
		counter >>= 1;
	}
	counter = 0x01;
	
	for(uint8_t i = 0; i < 8; i++)
	{
		counter <<= 1;
		PORTA = ~counter;
		_delay_ms(500);
	}
}

ISR(INT3_vect) // S/W : 3
{
	counter = 0; // 이진카운터 초기화
}

ISR(INT4_vect) // S/W : 0
{
	counter = 0x07;
	for(uint8_t i = 0; i < 6; i++)
	{
		PORTA = ~counter;
		_delay_ms(500);
		counter <<= 1;
	}
	PORTA = ~0xC1; // 더 깔끔한 방법이 있을거 같지만, 일단 이렇게 작성 하였다.
	_delay_ms(500);
	PORTA = ~0x83;
	_delay_ms(500);
	counter = 0x07;
	for(uint8_t i = 0; i < 8; i++)
	{
		PORTA = ~counter;
		_delay_ms(500);
		counter <<= 1;
	}	
}

ISR(INT6_vect) // S/W : 1
{
	counter = 0xE0;
	for(uint8_t i = 0; i < 6; i++)
	{
		PORTA = ~counter;
		_delay_ms(500);
		counter >>= 1;
	}
	PORTA = ~0x83;
	_delay_ms(500);
	PORTA = ~0xC1;
	_delay_ms(500);
	counter = 0xE0;
	for(uint8_t i = 0; i < 8; i++)
	{
		PORTA = ~counter;
		_delay_ms(500);
		counter >>= 1;

	}
}

