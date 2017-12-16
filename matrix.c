/****************************************************************************Å@
  5x7 led matrix display controller
  2013/06/29 @morecat_lab

   mcu=ATtiny2313

  MAPPING for LED
------------------------
  PB0: LED-C4
  PB1: LED-C5
  PB2: LED-R4
  PB3: LED-C3
  PB4: LED-R2
  PB5: LED-R1
  PB6: (NC)
  PB7: (NC)

  PD0: (NC) (reserved for RxD)
  PD1: (NC) (reserved for TxD)
  PD2: LED-C2
  PD3: LED-R5
  PD4: LED-R6
  PD5: LED-R7
  PD6: (NC)

  PA0: LED-R3
  PA1: LED-C1
  PA2: (RESET)

**************************************************************************** */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "font.c"

// #define PATTERN_1
//  #define PAT2
//  #define PAT2
//  #define PAT3
  #define PAT4
// #define PATTERN_2
#define PATTERN_3
// #define PATTERN_4

#define setPortBit0(port, bit) *(port) &= ~_BV(bit);
#define setPortBit1(port, bit) *(port) |= _BV(bit);

#define setPortBit(port, bit, val) { \
    if ((val) == 0) { setPortBit0((port), (bit)); } else	\
      { setPortBit1((port), (bit)); }				\
}
#define getPortBit(port, bit) ((*(port) & _BV(bit)) >> (bit))

#define flipPortBit(port, bit) (*(port) ^= _BV(bit))

#define Ncol 5
#define Nrow 7

#define MAX_CNT 15

/* port mapping info */
struct portInfo_t {
  volatile unsigned char *port;
  unsigned char bit;
};

struct portInfo_t LED_col [Ncol] = {
  { &PORTA, 1 }, /* COL1 */
  { &PORTD, 2 }, /* COL2 */
  { &PORTB, 3 }, /* COL3 */
  { &PORTB, 0 }, /* COL4 */
  { &PORTB, 1 }, /* COL5 */
};

struct portInfo_t LED_row [Nrow] = {
  { &PORTB, 5 }, /* ROW1 */
  { &PORTB, 4 }, /* ROW2 */
  { &PORTA, 0 }, /* ROW3 */
  { &PORTB, 2 }, /* ROW4 */
  { &PORTD, 3 }, /* ROW5 */
  { &PORTD, 4 }, /* ROW6 */
  { &PORTD, 5 }, /* ROW7 */
};

#ifdef PATTERN_1

#ifdef PAT1
#include "pat1.c"
#endif

#ifdef PAT2
#include "pat2.c"
#endif

#ifdef PAT3
#include "pat3.c"
#endif

#ifdef PAT4
#include "pat4.c"
#endif

#endif

void wait(int);

void setLedRow(unsigned char c) {
  unsigned char i;
  for (i = 0 ; i < Nrow ; i++) {
    if ((c & (1 << i)) != 0) {
      setPortBit0(LED_row[i].port,LED_row[i].bit); /* turn on LED ( = 0) */
    } else {
      setPortBit1(LED_row[i].port,LED_row[i].bit); /* turn off LED (= 1) */
    }
  }
}

void selectLedCol(unsigned char i) {
  setPortBit1(LED_col[i].port, LED_col[i].bit); /* select column (= 1) */
}

void unSelectLedCol(unsigned char i) {
  setPortBit0(LED_col[i].port, LED_col[i].bit); /* unselect column (= 0) */
}

char message [] = "I | MAKE  ";

/* main */
int main (void) {
  int k, i, j;
  int ch;
  int data;

  /* initialize port */
  DDRB  = 0b00111111;
  PORTB = 0b00000000;
  DDRD  = 0b00111100;
  PORTD = 0b00000000;
  DDRA  = 0b00000011;
  PORTA = 0b00000000;

#ifdef PATTERN_1
  /* display pattern */
  while(1) {  /* forever */
    for(k = 0 ; k < N_PATTERN ; k++) {
      for(j = 0 ; j < MAX_CNT ; j++) {
	for(i = 0; i < Ncol ; i++) {
	  setLedRow(vram[k][i]); /* set 1 row */
	  selectLedCol(i); /* LED ON */
	  wait(100);
	  unSelectLedCol(i); /* LED OFF */
	}
      }
    }
  }

#endif


#ifdef PATTERN_2
  /* display all character */
  while(1) {  /* forever */
    for (ch = ' ' ; ch < 96 ; ch++) {
      for (j = 0 ; j < MAX_CNT ; j++) {
	for (i = 0; i < Ncol ; i++) {
	  data = pgm_read_byte(&(FONT_CHARS[ch][i]));
	  setLedRow(data); /* set 1 row */
	  selectLedCol(i); /* LED ON */
	  wait(100);
	  unSelectLedCol(i); /* LED OFF */
	}
      }
    }
  }

#endif

#ifdef PATTERN_3
  /* display message text */
  while(1) {  /* forever */
    for (k = 0 ; k < sizeof(message) - 1; k++) {
      ch = message[k] - ' ';
      for (j = 0 ; j < MAX_CNT ; j++) {
	for (i = 0; i < Ncol ; i++) {
	  data = pgm_read_byte(&(FONT_CHARS[ch][i]));
	  setLedRow(data); /* set 1 row */
	  selectLedCol(i); /* LED ON */
	  wait(100);
	  unSelectLedCol(i); /* LED OFF */
	}
      }
    }
  }

#endif

#ifdef PATTERN_4

#ifdef MAX_CNT
#undef MAX_CNT
#endif
#define MAX_CNT 10
  /* scroll text */
  while(1) {  /* forever */
    for (k = 0 ; k < sizeof(message) - 1; k++) {
      for (j = 0 ; j < Ncol ; j++) {
	for (i = Ncol -1 ; i >= 0 ; i--) {
	  int u;
	  int ch, ix;
	  for (u = 0 ; u < MAX_CNT ; u++) {
	    ix = i + j;
	    if (ix >= Ncol) {
	      ix -= Ncol;
	      ch = message[k+1] - ' ';
	    } else {
	      ch = message[k] - ' ';
	    }
	    data = pgm_read_byte(&(FONT_CHARS[ch][ix]));
	    setLedRow(data); /* set 1 row */
	    selectLedCol(i); /* LED ON */
	    wait(10);
	    unSelectLedCol(i); /* LED OFF */
	  }
	}
      }
    }
  }
#endif

} /* main */

void wait(int c){ /* wait loop */
  uint16_t j;
  for(j=0;j<c;j++){
    j = j;         /* dummy */
  }
}
/* EOF */
