/*
 * LCD.c
 *
 * Created: 2025-12-11 오후 1:07:22
 *  Author: LK LAB
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "SPI.h"
#include "macro.h"
#include "LCD.h"

uint8_t text_cursor_x = 0;
uint8_t text_cursor_y = 0;

//---------------------- RS / RESET Functions ----------------------//
void RS_HIGH(void) {
	Temp_COMDATA_CON |= (1 << LCD_RS);
	UPDATE_COMDATA();
}

void RS_LOW(void) {
	Temp_COMDATA_CON &= ~(1 << LCD_RS);
	UPDATE_COMDATA();
}

void RESET_HIGH(void) {
	Temp_COMDATA_CON |= (1 << LCD_RESET);
	UPDATE_COMDATA();
}

void RESET_LOW(void) {
	Temp_COMDATA_CON &= ~(1 << LCD_RESET);
	UPDATE_COMDATA();
}


//---------------------- Low-level R/W Functions ----------------------//
void lcd_write_command(uint8_t cmd)
{
	RS_LOW();  // Command 모드
	spi_transmit(cmd);
}

void lcd_write_data(uint8_t data)
{
	RS_HIGH();  // Data 모드
	spi_transmit(data);
}

uint8_t lcd_read_data()
{
	uint8_t v;
	RS_HIGH();  // Data 모드
	v = spi_transmit(0x00);
	return v;
}

//---------------------- Low_Level Set Window Functions ----------------------//
void lcd_set_window_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	// Column window Set
	lcd_write_command(0x2A);
	lcd_write_data(x1 >> 8);
	lcd_write_data(x1 & 0xFF);
	lcd_write_data(x2 >> 8);
	lcd_write_data(x2 & 0xFF);
	
	// Page window Set
	lcd_write_command(0x2B);
	lcd_write_data(y1 >> 8);
	lcd_write_data(y1 & 0xFF);
	lcd_write_data(y2 >> 8);
	lcd_write_data(y2 & 0xFF);
	
	// Memory Write
	lcd_write_command(0x2C);
}

void lcd_set_window_read(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	// Column window Set
	lcd_write_command(0x2A);
	lcd_write_data(x1 >> 8);
	lcd_write_data(x1 & 0xFF);
	lcd_write_data(x2 >> 8);
	lcd_write_data(x2 & 0xFF);
	
	// Page window Set
	lcd_write_command(0x2B);
	lcd_write_data(y1 >> 8);
	lcd_write_data(y1 & 0xFF);
	lcd_write_data(y2 >> 8);
	lcd_write_data(y2 & 0xFF);
	
	// Memory read
	lcd_write_command(0x2E);
}

//---------------------- Initialization Functions ----------------------//
void lcd_gpio_init(void)
{
	// LCD 제어 핀들을 출력으로 설정 (RS, RESET)
	//DDR_RS_RST();
	// 초기 상태 설정
	RS_HIGH();
	RESET_HIGH();

}

void lcd_init(void)
{
	// GPIO 초기화
	lcd_gpio_init();
	
	// SPI 초기화
	spi_init();
	
	// LCD 리셋 시퀀스
	RESET_HIGH();
	_delay_ms(5);
	RESET_LOW();
	_delay_ms(15);
	RESET_HIGH();
	_delay_ms(15);
	
	spi_select();  // CS LOW
	
	// Power Control A ILI9341_POWERCONTROLA 
	lcd_write_command(0xCB);
	lcd_write_data(0x39);
	lcd_write_data(0x2C);
	lcd_write_data(0x00);
	lcd_write_data(0x34);
	lcd_write_data(0x02);
	
	// Power Control B ILI9341_POWERCONTROLB
	lcd_write_command(0xCF);
	lcd_write_data(0x00);
	lcd_write_data(0xC1);
	lcd_write_data(0x30);
	
	// Driver Timing Control A ILI9341_DRIVERTIMINGA
	lcd_write_command(0xE8);
	lcd_write_data(0x85);
	lcd_write_data(0x00);
	lcd_write_data(0x78);
	
	// Driver Timing Control B ILI9341_DRIVERTIMINGB
	lcd_write_command(0xEA);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	
	// Power on Sequence Control ILI9341_POWERONSEQ
	lcd_write_command(0xED);
	lcd_write_data(0x64);
	lcd_write_data(0x03);
	lcd_write_data(0x12);
	lcd_write_data(0x81);
	
	// Pump Ratio Control ILI9341_RUMPRATIO 
	lcd_write_command(0xF7);
	lcd_write_data(0x20);
	
	// Power Control 1
	lcd_write_command(0xC0);
	lcd_write_data(0x23);  // VRH[5:0]
	
	// Power Control 2
	lcd_write_command(0xC1);
	lcd_write_data(0x10);  // SAP[2:0];BT[3:0]
	
	// VCM Control 1
	lcd_write_command(0xC5);
	lcd_write_data(0x3e);  // Contrast
	lcd_write_data(0x28);
	
	// VCM Control 2
	lcd_write_command(0xC7);
	lcd_write_data(0x86);
	
	// Memory Access Control
	lcd_write_command(0x36);
	lcd_write_data(0xE8);
	
	// Pixel Format Set
	lcd_write_command(0x3A);
	lcd_write_data(0x55);  // 16bit/pixel
	
	// Frame Rate Control
	lcd_write_command(0xB1);
	lcd_write_data(0x00);
	lcd_write_data(0x18);
	
	// Display Function Control
	lcd_write_command(0xB6);
	lcd_write_data(0x08);
	lcd_write_data(0x82);
	lcd_write_data(0x27);
	
	// Exit Sleep
	lcd_write_command(0x11);
	_delay_ms(120);
	
	// Display ON
	lcd_write_command(0x29);
	lcd_write_command(0x2C);
	
	spi_deselect();  // CS HIGH
}

//---------------------- Clear Function ----------------------//
void lcd_clear(uint16_t color)
{
	uint16_t i, j;
	
	spi_select();
	lcd_set_window_write(0, 0, 319, 239);
	
	for(j = 0; j < 240; j++)
	{
		for(i = 0; i < 320; i++)
		{
			lcd_write_data(color >> 8);
			lcd_write_data(color & 0xFF);
		}
	}
	
	spi_deselect();
}

//---------------------- Write Dot Function ----------------------//
void lcd_write_dot(uint16_t x, uint16_t y, uint16_t c)
{
	spi_select();
	
	lcd_set_window_write(x, y, x, y);
	lcd_write_data(c >> 8);
	lcd_write_data(c & 0xFF);
	
	spi_deselect();
	
}
//---------------------- Read Pixel Function ----------------------//
uint16_t lcd_read_pixel(uint16_t x, uint16_t y)
{
	uint8_t r, g, b;
	
	spi_select();
	
	lcd_set_window_read(x, y, x, y);
	lcd_read_data(); // dummy data
	r = lcd_read_data();
	g = lcd_read_data();
	b = lcd_read_data();
	
	spi_deselect();
	
	uint16_t color565 = 0;
	color565 |= (r & 0xF8) << 8;  // R5 11111_000000_00000
	color565 |= (g & 0xFC) << 3;  // G6 00000_111111_00000
	color565 |= (b >> 3);         // B5 00000_111111_00000
	
	return color565;
}
//---------------------- Make cursor Function ----------------------//
void lcd_set_text_cursor(uint8_t tx, uint8_t ty) // 커서를 (tx, ty) 좌표로 설정하는 함수
{
	if (tx >= TEXT_COLS) tx = (TEXT_COLS - 1);
	if (ty >= TEXT_ROWS) ty = (TEXT_ROWS - 1);

	text_cursor_x = tx;
	text_cursor_y = ty;
}

void lcd_move_text_cursor(void) // 커서를 옆으로 한 칸 옮기는 함수
{
	if (text_cursor_x + 1 < TEXT_COLS) 
	{
		text_cursor_x++;
	} 
	else 
	{
		text_cursor_x = 0;
		if (text_cursor_y + 1 < TEXT_ROWS) 
		{
			text_cursor_y++;
		}
	}
}

void lcd_set_text_window_write(void) // 커서 위치에서 12*16 윈도우를 생성하는 함수
{
	uint16_t px = (uint16_t)text_cursor_x * CHAR_W;
	uint16_t py = (uint16_t)text_cursor_y * CHAR_H;

	lcd_set_window_write(
	px,
	py,
	(uint16_t)(px + (CHAR_W - 1)),
	(uint16_t)(py + (CHAR_H - 1))
	);
}
//---------------------- Write digit Function ----------------------//
void lcd_write_digit(uint16_t color, uint8_t digit, uint16_t bg)
{
	if (digit > 9) return;

	spi_select();

	// 16x16 윈도우 설정
	lcd_set_text_window_write();

	// 픽셀 출력 (y -> x)
	for (uint8_t row = 0; row < CHAR_H; row++)
	{
		uint8_t bit  = (row < 8) ? row : (uint8_t)(row - 8);
		uint8_t mask = (uint8_t)(1u << bit);

		for (uint8_t col = 0; col < CHAR_W; col++)
		{
			
			uint8_t b = (row < 8)
			? pgm_read_byte(&digit_font_16x16[digit][col])
			: pgm_read_byte(&digit_font_16x16[digit][16 + col]);

			uint16_t pix = (b & mask) ? color : bg;

			lcd_write_data((uint8_t)(pix >> 8));
			lcd_write_data((uint8_t)(pix & 0xFF));
		}
	}
	spi_deselect();

	lcd_move_text_cursor();
}

//---------------------- Write char in array Function ----------------------//
void lcd_write_array(uint16_t color, uint8_t idx, uint16_t bg)
{
	if (idx >= 8) {
		return;
	}

	const uint8_t *glyph = &control_font_16x16[idx][0];

	spi_select();
	lcd_set_text_window_write();

	for (uint8_t row = 0; row < CHAR_H; row++) {
		uint8_t bit  = (uint8_t)(row & 0x07);
		uint8_t mask = (uint8_t)(1u << bit);

		const uint8_t *row_base = glyph + ((row < 8) ? 0 : 16);

		for (uint8_t col = 0; col < CHAR_W; col++) {
			uint8_t b = pgm_read_byte(row_base + col);
			uint16_t pix = (b & mask) ? color : bg;

			lcd_write_data((uint8_t)(pix >> 8));
			lcd_write_data((uint8_t)(pix & 0xFF));
		}
	}

	spi_deselect();
	lcd_move_text_cursor();
}

//---------------------- Write string Function ----------------------//
void lcd_write_char(uint16_t color, char ch, uint16_t bg)
{
	if (ch == '\r') {
		return;
	}
	if (ch == '\n') {
		lcd_set_text_cursor(0, text_cursor_y++);
		return;
	}

	if (ch == ' ') {
		spi_select();
		lcd_set_text_window_write();
		for (uint16_t i = 0; i < (uint16_t)(CHAR_W * CHAR_H); i++) {
			lcd_write_data((uint8_t)(bg >> 8));
			lcd_write_data((uint8_t)(bg & 0xFF));
		}
		spi_deselect();
		lcd_move_text_cursor();
		return;
	}

	const uint8_t *glyph = 0;

	if (ch >= 'A' && ch <= 'Z')
	{
		glyph = &alphabet_up_font_16x16[(uint8_t)(ch - 'A')][0];
	}
	else if (ch >= 'a' && ch <= 'z')
	{
		glyph = &alphabet_low_font_16x16[(uint8_t)(ch - 'a')][0];
	}
	else if (ch >= '0' && ch <= '9')
	{
		glyph = &digit_font_16x16[(uint8_t)(ch - '0')][0];
	}
	else 
	{
		return;
	}

	spi_select();
	lcd_set_text_window_write();

	for (uint8_t row = 0; row < CHAR_H; row++) {
		uint8_t bit  = (uint8_t)(row & 0x07);
		uint8_t mask = (uint8_t)(1u << bit);

		const uint8_t *row_base = glyph + ((row < 8) ? 0 : 16);

		for (uint8_t col = 0; col < CHAR_W; col++) {
			uint8_t b = pgm_read_byte(row_base + col);
			uint16_t pix = (b & mask) ? color : bg;

			lcd_write_data((uint8_t)(pix >> 8));
			lcd_write_data((uint8_t)(pix & 0xFF));
		}
	}

	spi_deselect();
	lcd_move_text_cursor();
}

void lcd_write_string(uint16_t color, const char *s, uint16_t bg)
{
	while (*s) {
		lcd_write_char(color, *s++, bg);
	}
}


