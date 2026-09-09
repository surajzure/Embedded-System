#include <xc.h>
#include "matrix_keypad.h"

void init_matrix_keypad(void)
{
    
    RBPU = 0;
    
    TRISB = TRISB & 0x0F;  // not required
    
    TRISB = TRISB | 0x1E;
    
    PORTB = PORTB | 0xE0;
    
}
    
unsigned char scan_key(void)
{
    // ROW 1
    RW1 = 0;
    RW2 = RW3 = 1;
     
    if(COL1 == 0)
        return SW1;
    else if(COL2 == 0)
        return SW4;
    else if(COL3 == 0)
        return SW7;
    else if(COL4 == 0)
        return SW10;
    
    // ROW 2
    RW2 = 0;
    RW1 = RW3 = 1;
     
    
    if(COL1 == 0)
        return SW2;
    else if(COL2 == 0)
        return SW5;
    else if(COL3 == 0)
        return SW8;
    else if(COL4 == 0)
        return SW11;
    
    // ROW 3
    RW3 = 0;
    RW1 = RW2 = 1;
     
    if(COL1 == 0)
        return SW3;
    else if(COL2 == 0)
        return SW6;
    else if(COL3 == 0)
        return SW9;
    else if(COL4 == 0)
        return SW12;
    
    
    return 0xFF; // for no key pressed condition
    
    
}
        
unsigned char read_matrix_keypad(unsigned char triggering_method)
{
    static unsigned char once = 1;

    if(triggering_method == LEVEL)
    {
        return scan_key();
    }
    else if(triggering_method == EDGE)
    {
        unsigned char key = scan_key();
        
        if((key != 0xFF) && (once == 1))
        {
            once = 0;
            return key;
        }
        else if(key == 0xFF)
        {
            once = 1;
        }
        
    }
    return 0xFF;

}