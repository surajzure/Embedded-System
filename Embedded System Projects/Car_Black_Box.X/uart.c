#include <xc.h>
#include "uart.h"
void init_uart(void)
{
    
    
    TRISC6 = 0;
    TRISC7 = 1;
    // TXSTA: TRANSMIT STATUS AND CONTROL REGISTER            
    
    BRGH = 1;  // high speed selection
    SENDB = 0; // disable break character bit 
    SYNC = 0;  
    TXEN = 1;
    TX9 = 0;
    
    //  RCSTA: RECEIVE STATUS AND CONTROL REGISTER 
    
    CREN = 1;
    RX9 = 0;
    SPEN = 1;
    
    // BAUDCON: BAUD RATE CONTROL REGISTER     
    
    ABDOVF = 0;
    BRG16 = 0;
    WUE = 0;
    ABDEN = 0;
    
    SPBRG = 129;  // setting the baudrate as 9600
    
    TXIF = 0;
    RCIF = 0;
    
    
    
    
    return;
}

void putchar(unsigned char data)
{
    while(!TXIF);
    
    TXREG = data;
     
}

unsigned char getchar(void)
{
    while(!RCIF);
    
    return RCREG;
}

void puts(unsigned char *data)
{
    while(*data != '\0')
    {
        putchar(*data);
        data++;
    }
}