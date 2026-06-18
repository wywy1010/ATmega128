#ifndef MACRO_H_
#define MACRO_H_

#include <avr/io.h>

// 색상 정의
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// 하드웨어 주소 정의
#define LED_CON             (*((volatile unsigned char *)0x8000))
#define FND_RELAY_BUZ_CON   (*((volatile unsigned char *)0x8100))
#define COMDATA_CON         (*((volatile unsigned char *)0x8200)) // LCD 제어/데이터
#define DOT_YELLOW_CON      (*((volatile unsigned char *)0x8300))
#define STEPM_CON           (*((volatile unsigned char *)0x8400))

// 쉐도우 레지스터 선언
extern volatile uint8_t Temp_COMDATA_CON;

// 변수 값을 실제 외부 메모리 주소로 출력
#define UPDATE_COMDATA()    (COMDATA_CON = Temp_COMDATA_CON)

#endif /* MACRO_H_ */
