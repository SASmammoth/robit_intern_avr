/*
 * Task-02.c
 *
 * Created: 2026-07-29 오후 8:56:18
 * Author : SASmammoth
 *
 * Day_02 2번 과제
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

volatile uint8_t counter = 0; // 인터럽트와 변수를 공유하기 위함.



int main(void)
{
	DDRA = 0xFF;
	DDRF =  0x00;
	ADMUX = 0x40;
	ADCSRA = 0x87;
	
	
	i2c_master_init(100000);
	_delay_ms(100);

	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS);
	_delay_ms(50);
	lq_turnOnBacklight(&device);
	lq_clear(&device);              

	lq_setCursor(&device, 0, 0);    
	lq_print(&device, "JKM");

	lq_setCursor(&device, 1, 0);    
	
	
	
	unsigned int adcValue = 0;
	unsigned char channel = 0x00;
	unsigned int adc_Volt_100;
	
	unsigned int LED_num = 0;
	unsigned int LED_temp = 0;

	while (1)                       
	{
		
		
		ADMUX = 0x40 | channel;
		ADCSRA |= 0x40;
		while((ADCSRA&0x10) == 0);
		adcValue = ADC;
		
		
		adc_Volt_100 = (unsigned long)adcValue * 500 / 1024;
		
		
		
		char buf_[15];
		sprintf(buf_, "%4u %u.%2uV", adcValue, adc_Volt_100 / 100 ,  adc_Volt_100 % 100);
		lq_setCursor(&device, 1, 0);
		lq_print(&device, buf_);
		
		LED_num = adcValue * 8 / 1024;
		
		LED_temp = 1;
		for(int i = 0; i < LED_num; i++)
		{
			LED_temp <<= 1;
			LED_temp +=1;
		}
		PORTA = ~LED_temp;
			
		
		
		
		_delay_ms(100);
		
	}
}