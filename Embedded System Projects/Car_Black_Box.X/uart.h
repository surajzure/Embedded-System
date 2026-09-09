#ifndef UART_H
#define	UART_H

void init_uart(void);
void putchar(unsigned char data);
void puts(unsigned char *data);
unsigned char getchar(void);

#endif	/* UART_H */

