#ifndef F_CPU
#define F_CPU 16000000UL 
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "LCD.h"
#include "SPI.h"
#include "TOUCH.h"
#include "macro.h"

volatile uint8_t Temp_COMDATA_CON = 0x00;

/* ================== UART1 ================== */
static int usart_putchar(char c, FILE *stream);
static FILE uart_output = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

void usart_init(void)
{
	UCSR1A = 0x00;
	UCSR1B = (1<<TXEN1);     // TX only
	UCSR1C = 0x06;           // 8N1
	UBRR1H = 0x00;
	UBRR1L = 0x67;           // 9600
}
static int usart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	usart_putchar('\r', stream);
	while (!(UCSR1A & (1<<UDRE1)));
	UDR1 = c;
	return 0;
}

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
	stdout = &uart_output;
	usart_init();
	
	EXT_IO_INIT();
	UPDATE_COMDATA();
	
	lcd_init();
	touch_init();

	lcd_clear(BLACK);
	printf("System Ready\n");
	while(1){

		if (!(PIND & (1 << PIND7)))
		{
		touch_read_xy();
		if (touch_error_flag) {
			printf("TOUCH ERROR\n");
			touch_error_flag = 0;
			} else {
			printf("Touch axis : %u, %u\n", x, y);
		}
		}

		_delay_ms(20); //디바운싱
		}

}

