/*
 * Day03_Task03.c
 *
 * Created: 2026-07-31 오후 9:23:01
 * Author : SASmammoth
 */ 


#define F_CPU 16000000
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "liquid_crystal_i2c.h"

unsigned char Uart_Getch_PC(void);
void Uart_Putch_PC(uint8_t PutData);

void Getch_Dcell(void); // 다이나믹셀의 출력 pc에 띄우기
void Putch_Dcell(uint8_t *PutData, uint8_t size); // 다이나믹셀에 데이터 넣기


void push_Pint(void); // 다이나믹셀에 ping을 넣는 부분

void print_serial(char *print_); // PC시리얼 화면에 문자열 전송

void Torque_Enable(uint8_t id, uint8_t on); // 토크설정 LLM 이용
void Set_Goal_Position(uint8_t id, uint32_t position); // 위치 설정 LLM 이용
void set_moter_speed(uint8_t id, uint16_t speed); // 모터 속도 설정, 내가 작성함
unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size);

volatile uint16_t moter_speed_input = 1;
volatile uint8_t speed_update_flag = 0;   // 속도가 바뀌었는지 확인

int main(void)
{
	UBRR1H = 0x00; // PC와의 통신
	UBRR1L = 103; // 9600bps 사용 
	UCSR1A = 0x20; // 송신, 수신 상태비트 초기화
	UCSR1B = 0x18; // 송신, 수신 기능 활성
	UCSR1C = 0x06; // START 1비트/ DATA 8비트/ STOP 1비트
	DDRD = 0x08; // RX-TX핀 입출력 설정 | D2 입력(RX), D3 출력(TX)
	UCSR1B |= 0x80; // 수신완료 인터럽트 활성화 
	
	
	UBRR0H = 0x00; // 다이나믹셀 과의 통신
	UBRR0L = 34; // 16MHz, U2X=1 → 57600bps, 오차 0.79% LLM 사용
	UCSR0A = 0x22; // 송신, 수신 상태비트 초기화
	UCSR0B = 0x18; // 송신, 수신 기능 활성
	UCSR0C = 0x06; // START 1비트/ DATA 8비트/ STOP 1비트
	DDRE = 0x01; // RX-TX핀 입출력 설정 | E0 입력(RX), E1 출력(TX)
	
	DDRA = 0xFF;
	
	EIMSK = 0x10; // INT4 사용
	EICRA = 0x00;
	EICRB = 0x02; // 하강엣지 사용
	sei(); // 인터럽트 실행
	
	DDRE |= 0x04; // PE2 MAX485 송수신 전환으로 설정
	
	PORTA = ~0x00; // 모든 LED 끄기;
	
	ADMUX = 0x40; // AVCC 전압 사용 ADC0번을 단일종단이득으로 설정
	ADCSRA = 0x87; // ADC 활성화 및 분주비 128 사용
	
	
	
	i2c_master_init(100000); // i2c 통신속도 초기화
	_delay_ms(100);

	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS); // LCD 생성
	_delay_ms(50);
	lq_turnOnBacklight(&device);
	lq_clear(&device);

	lq_setCursor(&device, 0, 0);
	
	char recvData;
	unsigned char channel = 0x00;
	uint16_t adcValue = 16;
	
	Torque_Enable(1, 1);         // 토크 on
	set_moter_speed(1, moter_speed_input);
	
		
	
	while (1)
	{	
		ADMUX = 0x40 | channel;
		ADCSRA |= 0x40; // 변환 시작
		while((ADCSRA&0x10) == 0); // ADC가져올때 까지 대기
		adcValue = ADC; // ADC값 저장
		ADCSRA |= 0x10;
		
		if(speed_update_flag)
		{
			set_moter_speed(1, moter_speed_input);
			speed_update_flag = 0;
		}
		
		char buf_[16];
		char num_[8];

		itoa(adcValue, num_, 10);       // 숫자를 문자열로
		strcpy(buf_, "pos : ");
		strcat(buf_, num_);
		strcat(buf_, "    ");
		
		lq_setCursor(&device, 0, 0);
		lq_print(&device, buf_);

		itoa(moter_speed_input, num_, 10);
		strcpy(buf_, "spd : ");
		strcat(buf_, num_);
		strcat(buf_, "    ");
		
		lq_setCursor(&device, 1, 0);
		lq_print(&device, buf_);
		
		Set_Goal_Position(1, adcValue); // 모터 돌리기
		_delay_ms(1000); // 딜레이를 줄이면 명령이 씹히는 문제가 발생함
	}
}

void set_moter_speed(uint8_t id, uint16_t speed) // 내가 작성
{
// 0xFF 0xFF 0xFD 0x00 0x01(id) 0x09(data크기가 4이므로 1 + 2 + 4 + 2) 0x00 0x03 0x68(주소) 0x00 data4개
	uint8_t packet[16];
	packet[0] = 0xFF;
	packet[1] = 0xFF;
	packet[2] = 0xFD;
	packet[3] = 0x00;
	packet[4] = id;         // ID
	packet[5] = 0x09;       // LEN_L 
	packet[6] = 0x00;       // LEN_H
	packet[7] = 0x03;       // INST = Write
	packet[8] = 0x70;       // ADDR_L (112) Goal Velocity 주소 메뉴얼에서 가져옴
	packet[9] = 0x00;       // ADDR_H
	packet[10] = speed & 0xFF;          // DATA (little-endian)
	packet[11] = (speed >> 8) & 0xFF;
	packet[12] = (speed >> 16) & 0xFF;
	packet[13] = (speed >> 24) & 0xFF;

	unsigned short crc = update_crc(0, packet, 14); // 헤더~데이터까지 14바이트
	packet[14] = crc & 0xFF;         // CRC_L
	packet[15] = (crc >> 8) & 0xFF;  // CRC_H
	
	// 형식이 Set_Goal_Position 랑 비슷하여 딱히 수정할게 많이 없었음

	Putch_Dcell(packet, 16);
}

ISR(USART1_RX_vect) // PC로 부터 수신되면
{
	char input_num = UDR1;
	if(input_num >= '0' && input_num <= '9')
	{
		moter_speed_input = (input_num - '0') * 30;
		speed_update_flag = 1; // 플래그 세이기
	}
}

void push_Pint(void) // 학습용 push_Ping
{
	uint8_t ping_list[10] = {0xFF, 0xFF, 0xFD, 0x00, 0x01, 0x03, 0x00, 0x01, 0x19, 0x4E}; //ping 패킷
	PORTE = 0x04; // MAX485 송신으로 전환
	UCSR0B &= ~(1 << RXEN0);   // RX 비활성화 (에코 차단) LLM
	UCSR0A |= (1<<TXC0); // Tx 클리어 LLM
	
	Putch_Dcell(ping_list, 10);
	
	uint16_t timeout = 0; // 무한반복 방지, LLM
	while(!(UCSR0A & (1 << TXC0))) // 입력이 끝날때 까지 대기
	{if(++timeout > 60000) break;} // 타임아웃 후 브래이크
		
	PORTE = 0x00; // 수신전환
	UCSR0B |= (1 << RXEN0);   // RX 재활성화LLM
	
	
	Getch_Dcell();
}

void Getch_Dcell(void)
{ // 다이나믹셀의 출력 pc에 띄우기
	for(uint8_t i = 0; i < 14; i++)
	{
		uint16_t timeout = 0;
		while(!(UCSR0A & (1 << RXC0))) // 수신이 끝날때 까지 대기
			if(++timeout > 60000) return;
		Uart_Putch_PC(UDR0); // PC로 출력
	}
	Uart_Putch_PC('\r'); Uart_Putch_PC('\n');
}


void Putch_Dcell(uint8_t *PutData, uint8_t size)
{ // 다이나믹셀에 데이터 넣기
	
	// 송신 (에코 방지 포함) LLM의 추천
	PORTE = 0x04;
	UCSR0B &= ~(1 << RXEN0);
	UCSR0A |= (1 << TXC0);
		
	for(uint8_t i = 0; i < size; i++)
	{
		while(!(UCSR0A & (1 << UDRE0))); // 입력이 끝날때 까지 대기
		UDR0 = PutData[i]; // 다음 데이터 넣기
	}
	
	uint16_t t = 0; // LLM의 수정 : 모든 바이트가 전송될 때 까지 대기
	while(!(UCSR0A & (1 << TXC0)))
	if(++t > 60000) break;
	
	PORTE = 0x00; // 에코 방지 해제
	UCSR0B |= (1 << RXEN0); // RX켜기
}

unsigned char Uart_Getch_PC(void) // 시리얼을 통해 문자 입력
{
	while(!(UCSR1A & (1 << RXC1))); // 입력이 끝날때 까지 대기
	return UDR1;					// 반환
}

void Uart_Putch_PC(uint8_t PutData) // 시리얼을 통해 문자 출력
{
	while(!(UCSR1A & (1 << UDRE1)));
	UDR1 = PutData;
}

unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size)
{ // 공식문서를 LLM 사용하여 가져옴
	unsigned short i, j;
	static const unsigned short crc_table[256] = {
		0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
		0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
		0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
		0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
		0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
		0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
		0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
		0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
		0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
		0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
		0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
		0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
		0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
		0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
		0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
		0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
		0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
		0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
		0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
		0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
		0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
		0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
		0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
		0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
		0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
		0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
		0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
		0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
		0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
		0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
		0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
		0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
	};

	for (j = 0; j < data_blk_size; j++)
	{
		i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}
	return crc_accum;
}

void Set_Goal_Position(uint8_t id, uint32_t position)
{ // LLm 사용
	uint8_t packet[16];
	packet[0] = 0xFF;
	packet[1] = 0xFF;
	packet[2] = 0xFD;
	packet[3] = 0x00;
	packet[4] = id;         // ID
	packet[5] = 0x09;       // LEN_L (파라미터 6 + 3)
	packet[6] = 0x00;       // LEN_H
	packet[7] = 0x03;       // INST = Write
	packet[8] = 0x74;       // ADDR_L (116)
	packet[9] = 0x00;       // ADDR_H
	packet[10] = position & 0xFF;          // DATA (little-endian)
	packet[11] = (position >> 8) & 0xFF;
	packet[12] = (position >> 16) & 0xFF;
	packet[13] = (position >> 24) & 0xFF;

	unsigned short crc = update_crc(0, packet, 14); // 헤더~데이터까지 14바이트
	packet[14] = crc & 0xFF;         // CRC_L
	packet[15] = (crc >> 8) & 0xFF;  // CRC_H



	Putch_Dcell(packet, 16);
}

void Torque_Enable(uint8_t id, uint8_t on)
{ // LLM 사용
	uint8_t packet[13];
	packet[0]=0xFF; packet[1]=0xFF; packet[2]=0xFD; packet[3]=0x00;
	packet[4]=id;
	packet[5]=0x06; packet[6]=0x00;   // LEN = 파라미터3 + 3
	packet[7]=0x03;                   // Write
	packet[8]=0x40; packet[9]=0x00;   // ADDR 64 (Torque Enable)
	packet[10]=on;                    // 1=on, 0=off
	unsigned short crc = update_crc(0, packet, 11);
	packet[11]=crc&0xFF; packet[12]=(crc>>8)&0xFF;
	
	Putch_Dcell(packet,13);
}

void print_serial(char *print_) // 문자열을 받아 시리얼로 출력하는 함수이다.
{
	while(*print_!= '\0')
	Uart_Putch_PC(*print_++);
}



