/*
 * Task-02.c
 *
 * Created: 2026-07-29 오후 3:32:59
 * Author : SASmammoth
 * 
 * 1일차 과제 2번.
 */ 

#define  F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main(void)
{
    /* Replace with your application code */
	DDRA = 0xFF;
	DDRC = 0x00;
	DDRD = 0x00;
	
	EIMSK = 0x0C; // 2, 3번 인터럽트 빈 (PD2, PD3) 사용
	
	EICRA = 0x03; // 하강 엣지 클럭 사용
	EICRB = 0x00;
	
	unsigned char SW_C = 0, SW_D = 0; // 스위치가 풀업 방식이므로 입력 레지스터 사용을 용의하게 하기위해 사용
	unsigned char MS_c = 0x03; // 0000 0011을 의미한다.
	unsigned char MS_d = 0x0C; // 0000 1100을 의미한다.
	
	sei();
	
	while (1)
	{
		
		
		SW_C = PINC ^ MS_c; // 마스크와 베타적 논리 합을 이용하여 깔끔하게 스위치 입력 확인
		SW_D = PINC ^ MS_d;
		PORTA = 0x00;
		if(SW_C & (1 << PINC0) && SW_C & (1 << PINC1)) // 2개 다 누를때
		{
			PORTA = 0x00;
			_delay_ms(500);
		}
		else if(SW_C & (1 << PINC0)) // SW 1이 눌리면
		{
			PORTA = 0xF0;
			_delay_ms(500);
		}
		else if(SW_C & (1 << PINC1)) // SW 2가 눌리면
		{
			PORTA = 0x0F;
			_delay_ms(500);
		}
		else // 기본
		{
			PORTA = 0xFF;
			_delay_ms(500);
			PORTA = 0x00;
			_delay_ms(500);
		}
		
	}
}

ISR(INT2_vect) // 인터럽트 3
{
	PORTA = 0xFF;
	
	char temp_ = 0x01;
	for(char i = 0; i < 8; i++) // 좌측 이동
	{
		PORTA = temp_ ^ 0xFF;
		_delay_ms(100);
		temp_ <<= 1;
	}
	temp_ = 0x80;
	for(char i = 0; i < 8; i++) // 반대로
	{
		PORTA = temp_ ^ 0xFF;
		_delay_ms(100);
		temp_ >>= 1;
	}
	
}

ISR(INT3_vect) // 인터럽트 4
{
	PORTA = 0xFF;
	
	char temp_ = 0x80;
	for(char i = 0; i < 8; i++) // 우측 이동
	{
		PORTA = temp_ ^ 0xFF;
		_delay_ms(100);
		temp_ >>= 1;
	}
	
	temp_ = 0x01;
	for(char i = 0; i < 8; i++) // 반대로
	{
		PORTA = temp_ ^ 0xFF;
		_delay_ms(100);
		temp_ <<= 1;
	}
	
	
}

