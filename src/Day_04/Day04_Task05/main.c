/*
 * Day04_Task05.c
 *
 * Created: 2026-08-02 오후 5:30:36
 * Author : SASmammoth
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

uint8_t get_angle(uint8_t prev_angle); // 각도를 반환하는 함수
void print_serial(char *print_);
unsigned char Uart_Getch(void);
void Uart_Putch(unsigned char PutData);

volatile uint8_t count20ms = 0;
volatile uint16_t count180deg = 0;

volatile uint8_t prev_angle = 90; // 각도



int main(void)
{
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
	
	PORTB = 0;
	
	sei(); // 인터럽트 시작
	
	
    /* Replace with your application code */
    while (1) 
    {
		prev_angle = get_angle(prev_angle);
    }
}

uint8_t get_angle( uint8_t prev_angle)
{
	char temp_[4]; // 임시 버퍼
	
	print_serial("You must 3digits ex) 090\r\n");
	
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
	print_serial("\r\nok\r\n");
	return temp_int;
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

