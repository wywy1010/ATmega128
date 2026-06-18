/*
SPI.h
*/
#ifndef SPI_H_
#define SPI_H_


/* ================== SPI PIN ================== */
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB

#define SPI_CS			2	// PA2

#define SPI_SCK         1   // PB1
#define SPI_MOSI        2   // PB2
#define SPI_MISO        3   // PB3


/* ================== SPI_LCD LOW LEVEL ================== */
void spi_init(void);				// SPI 하드웨어 설정
uint8_t spi_transmit(uint8_t data);	// 송신 및 수신
void spi_select(void);				// 슬레이브 선택 (SS Low)
void spi_deselect(void);			// 슬레이브 해제 (SS High)



#endif /* SPI_H_ */
