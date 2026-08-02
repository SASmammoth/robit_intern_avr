/*
 * Day04_Task04.c
 *
 * Created: 2026-08-03 오전 12:03:31
 * Author : SASmammoth
 */ 


#define F_CPU 16000000
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define MAX_MOV_AVG_NUM 10 //이동평균 개수

void print_serial(char *print_);
unsigned char Uart_Getch(void);
void Uart_Putch(unsigned char PutData);



int main(void)
{
	UBRR1H = 0x00;
	UBRR1L = 103; // 9600bps 사용
	UCSR1A = 0x20; // 송신, 수신 상태비트 초기화
	UCSR1B = 0x18; // 송신, 수신 기능 활성화
	UCSR1C = 0x86;
	
	ADMUX = 0x47; // AVCC 전압 사용 ADC7번을 단일종단이득으로 설정
	ADCSRA = 0x87; // ADC 활성화 및 분주비 128 사용
	unsigned char channel = 0x47; // ADC 7번 사용
	uint16_t adcValue = 0;
	
	double length = 0;
	double voltage = 0;
	
	double moving_avg_list[MAX_MOV_AVG_NUM] = {0, }; // 이동평균을 계산하기 위한 리스트. 이동평균은 제귀적으로 구할 수 없기 때문
	double moving_avg = 0;
	int index = 0;
	double avg_length = 0; // 이동평균의 ROW데이터
	double avg_voltage = 0; // 이동평균의 전압
	
	
    /* Replace with your application code */
    while (1) 
    {
		ADMUX = 0x40 | channel;
		ADCSRA |= 0x40; // 변환 시작
		while((ADCSRA&0x10) == 0); // ADC가져올때 까지 대기
		adcValue = ADC; // ADC값 저장
		ADCSRA |= 0x10;
		
		voltage = adcValue * (5.0/1024.0); // 전압을 먼저 구함
		
		char prt_chr[80] = ""; // 출력 문자열
		length = 60.495 * pow(voltage, -1.1904); // 인터넷에서 가져온 가장 많이쓰인다는 가장 많이 사용되는 근사공식
		
		moving_avg_list[index] = adcValue; // 이동평균 리스트를 저장함
		index++;
		if(index == MAX_MOV_AVG_NUM)
			index = 0;
		
		moving_avg = 0; // 이동평균값 초기화
		for(int i = 0; i < MAX_MOV_AVG_NUM; i++)
		{
			moving_avg += moving_avg_list[i];
		}
		moving_avg /= MAX_MOV_AVG_NUM;
		
		avg_voltage = moving_avg * (5.0/1024.0); // 전압을 먼저 구함
		
		
		avg_length = 60.495 * pow(avg_voltage, -1.1904); // 이동평균의 길이
		
		
		sprintf(prt_chr, "ROW : %4d | FILTERD : %4d | ROW_DIS : %3dcm | FIL_DIS : %3dcm \r\n",adcValue, (int)moving_avg, (int) length, (int) avg_length);
		
		print_serial(prt_chr);
		
		_delay_ms(300);
		
		
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
