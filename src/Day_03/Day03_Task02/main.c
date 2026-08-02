/*
 * Day03_Task02.c
 *
 * Created: 2026-07-31 오후 4:58:38
 * Author : SASmammoth
 *
 * 송수신 부는 예제코드를 이용하였다.
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char Uart_Getch(void);
void Uart_Putch(unsigned char PutData);

void print_serial(char *); // 문자열을 받아 시리얼로 출력하는 함수이다.

volatile uint8_t is_LEDon = 0;

int main(void)
{
	UBRR1H = 0x00;
	UBRR1L = 103; // 9600bps 사용
	
	UCSR1A = 0x20; // 송신, 수신 상태비트 초기화
	//0010 0000
	
	UCSR1B = 0x18; // 송신, 수신 기능 활성화
	//0001 1000
	
	UCSR1C = 0x06; // START 1비트/ DATA 8비트/ STOP 1비트
	// 0000 0110
	
	// RX-TX핀 입출력 설정
	// D2 입력(RX), D3 출력(TX)
	DDRD = 0x08;
	
	DDRA = 0xFF;
	
	
	EIMSK = 0x10; // INT4 사용
	EICRA = 0x00; 
	EICRB = 0x02; // 하강엣지 사용
	sei(); // 인터럽트 실행
	
	PORTA = ~0x00; // 모든 LED 끄기;
	
	uint8_t recvData_int; // 입력 문자를 정수로 바꾼 값
	char recvData = 0; // 입력 문자
	
	uint8_t LED_num = 0; // 발광할 LED
	
    /* Replace with your application code */
    while (1) 
    {
		recvData = Uart_Getch();	// 1byte 데이터 수신
		
		recvData_int = recvData - '0'; // 문자를 숫자로 변환
		
		if(recvData_int > 9) // 예외처리
		{
			print_serial("error\r\n");
		}
		else if (recvData_int == 8) // LED좌측으로 이동
		{// LED_num을 바꾸어 실행
			if(LED_num == 0) LED_num = 7;
			else LED_num--;
			print_serial("LEFT\r\n"); // 'LEFT' 문자열 전송
		}
		else if (recvData_int == 9)// LED 우측으로 이동
		{
			if(LED_num == 7) LED_num = 0;
			else LED_num++;
			print_serial("RIGHT\r\n"); // 'RIGHT' 문자열 전송
		}
		else // 0~7의 입력은 LED 번호 바꾸고, 
		{
			LED_num = recvData_int;
			Uart_Putch(recvData); // 숫자 n출력
			print_serial(" LED on\r\n");
			is_LEDon = 1; // LED 켜기
		}
		
		if (is_LEDon)
			PORTA = ~(1 << LED_num); // LED 출력
		else
			PORTA = ~0x00; // LED 꺼짐
	}
}

void print_serial(char *print_) // 문자열을 받아 시리얼로 출력하는 함수이다.
{
	while(*print_!= '\0')
		Uart_Putch(*print_++);
}


unsigned char Uart_Getch(void) // 시리얼을 통해 문자 입력
{
	while(!(UCSR1A & (1 << RXC1))); // 입력이 끝날때 까지 대기
	return UDR1;					// 반환
}

void Uart_Putch(unsigned char PutData) // 시리얼을 통해 문자 출력
{
	while(!(UCSR1A & (1 << UDRE1)));
	UDR1 = PutData;
}

ISR(INT4_vect)
{
	is_LEDon = 0; // 리셋
	PORTA = ~0x00;
	
	print_serial("RESET\r\n"); // 리셋 출력
}

