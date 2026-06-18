/*
 * SPI.c
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "SPI.h"
#include "macro.h"

void spi_init(void)
{
	// SCK, MOSI output
	SPI_DDR |= (1 << DDB0) | (1<<SPI_SCK) | (1<<SPI_MOSI);
	// MISO input
	SPI_DDR &= ~(1<<SPI_MISO);

	// SS high
	spi_deselect();

	// SPI enable, master, mode0
	SPCR = (1<<SPE) | (1<<MSTR);
	SPCR &= ~((1<<CPOL) | (1<<CPHA));

	// Clock = F_CPU/2
	SPCR &= ~((1<<SPR1) | (1<<SPR0));   // SPR1=0, SPR0=0
	//SPSR |= (1<<SPI2X);                 // double speed
}


uint8_t spi_transmit(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	return SPDR;
}

void spi_select(void) {
	Temp_COMDATA_CON &= ~(1 << SPI_CS); // CS Low
	UPDATE_COMDATA();
}

void spi_deselect(void) {
	Temp_COMDATA_CON |= (1 << SPI_CS);  // CS High
	UPDATE_COMDATA();
}

