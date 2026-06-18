/*
TFT LCD에 지정한 문자열을 띄우면서
1초마다 배경색을 바꾸는 예제
*/

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif

#define BLACK   0x0000
#define RED     0xF800
#define ORANGE  0xFD00
#define YELLOW  0xFFE0
#define GREEN   0x07E0
#define BLUE    0x001F
#define PURPLE  0xB81F
#define CYAN    0x07FF
#define MAGENTA 0xF81F

#define WHITE   0xFFFF

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "LCD.h"
#include "SPI.h"
#include "macro.h"

volatile uint8_t Temp_COMDATA_CON = 0x00;

void EXT_IO_INIT()
{
	MCUCR = 0X80; // 외부메모리 제어 및 I/O 제어 활성화
	LED_CON = 0X00;
	FND_RELAY_BUZ_CON = 0X00;
	COMDATA_CON = 0X00;
	DOT_YELLOW_CON = 0X00;
	STEPM_CON = 0X00;
}

int main(void)
{
	uint16_t color[8] ={RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA, WHITE};
	EXT_IO_INIT();
	UPDATE_COMDATA();
	lcd_init();
	
	while(1){
		for(int i=0; i<8; i++)
		{
			lcd_clear(color[i]);
			lcd_set_text_cursor(0,2);
			lcd_write_string(BLACK, "    intelligent", color[i]);
			lcd_set_text_cursor(0,4);
			lcd_write_string(BLACK, "  LK EMBEDDED LAB", color[i]);
			lcd_set_text_cursor(0,7);
			lcd_write_string(BLACK, "Arduino AVR ARM FPGA", color[i]);
			lcd_set_text_cursor(0,9);
			lcd_write_string(BLACK, "    Development", color[i]);
			lcd_set_text_cursor(0,11);
			lcd_write_string(BLACK, "  Evaluation board", color[i]);
			_delay_ms(1000);	
		}

	}

}

