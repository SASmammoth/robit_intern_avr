/*
 * Day03_Task03.c
 *
 * Created: 2026-08-01 오전 4:44:37
 * Author : SASmammoth
 */ 
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRD = 0x08; // PD3 출력모드로 바꿈
	PORTD = 0x08;// STOP모드로 변경
	_delay_us(1000);
		
    /* Replace with your application code */
    while (1) 
	{		
		{	// H 0100 1000 전송 함수 등으로 작성할수도 있지만, UART통신 학습 용도로 날것으로 작성함
			PORTD = 0x00;	// start
			_delay_us(104); // 9600bps 의 폭은 약 1/9600 = 104µs 정도이다.
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x00;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x00;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// e 0110 0101 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x08;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x00;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// l 0110 1100 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// l 0110 1100 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// o 0110 1111 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x08;	// 0
			_delay_us(104);
			PORTD = 0x08;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// ' ' 0010 0000 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x00;	// 2
			_delay_us(104);
			PORTD = 0x00;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x00;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// w 0111 0111 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x08;	// 0
			_delay_us(104);
			PORTD = 0x08;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x00;	// 03
			_delay_us(104);
			PORTD = 0x08;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// o 0110 1111 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x08;	// 0
			_delay_us(104);
			PORTD = 0x08;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// r 0111 0010 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x08;	// 1
			_delay_us(104);
			PORTD = 0x00;	// 2
			_delay_us(104);
			PORTD = 0x00;	// 03
			_delay_us(104);
			PORTD = 0x08;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// d 0110 0100 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x00;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x08;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// ! 0010 0001 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x08;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x00;	// 2
			_delay_us(104);
			PORTD = 0x00;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x08;	// 5
			_delay_us(104);
			PORTD = 0x00;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// \r 0000 1101 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x08;	// 0
			_delay_us(104);
			PORTD = 0x00;	// 1
			_delay_us(104);
			PORTD = 0x08;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x00;	// 5
			_delay_us(104);
			PORTD = 0x00;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		{	// \n 0000 1010 전송
			PORTD = 0x00;	// start
			_delay_us(104);
			PORTD = 0x00;	// 0
			_delay_us(104);
			PORTD = 0x08;	// 1
			_delay_us(104);
			PORTD = 0x00;	// 2
			_delay_us(104);
			PORTD = 0x08;	// 03
			_delay_us(104);
			PORTD = 0x00;	// 04
			_delay_us(104);
			PORTD = 0x00;	// 5
			_delay_us(104);
			PORTD = 0x00;	// 6
			_delay_us(104);
			PORTD = 0x00;	// 7
			_delay_us(104);
			PORTD = 0x08;	// ENd
			_delay_us(1040);
		}
		_delay_ms(1000);
    }
}

