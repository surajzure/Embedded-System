#ifndef MATRIX_KEYPAD_H
#define	MATRIX_KEYPAD_H

#define RW1       PORTBbits.RB5
#define RW2       PORTBbits.RB6
#define RW3       PORTBbits.RB7

#define COL1      PORTBbits.RB1
#define COL2      PORTBbits.RB2
#define COL3      PORTBbits.RB3
#define COL4      PORTBbits.RB4

#define HI        1
#define LOW       0

#define SW1       1
#define SW2       2
#define SW3       3
#define SW4       4
#define SW5       5
#define SW6       6
#define SW7       7
#define SW8       8
#define SW9       9
#define SW10      10
#define SW11      11
#define SW12      12

#define EDGE     1
#define LEVEL    0
void init_matrix_keypad(void);
unsigned char read_matrix_keypad(unsigned char triggering_method);
unsigned char scan_key(void);



#endif	/* MATRIX_KEYPAD_H */

