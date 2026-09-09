#include <xc.h>
#include "black_box.h"
#include "clcd.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "matrix_keypad.h"
#include "external_eeprom.h"
#include "uart.h"

#define _XTAL_FREQ 20000000

unsigned char gear[8][3] = {"NU","G1","G2","G3","G4","G5","RE ","CC"}; // make it global 
extern unsigned char key;

unsigned char clock_reg[3];
unsigned char time[9];
char menu[4][13]={"View_Log","Clear_Log","Set_Time","Download_Log"};
static unsigned char menu_pos = 0;
static unsigned char arrow_pos = 0;

static unsigned int count = 0; 
static unsigned int log_index = 0;

unsigned char gear_index = 0; // GEAR START FROM NETURAL 
unsigned char speed_data[3];

unsigned char address;
unsigned char log_uart[17];
unsigned char index_uart = 0;

// for write function
unsigned char eeprom_addr = 0x00;






static void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	if (clock_reg[0] & 0x40)
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	else
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
        
    clcd_print(time, LINE2(0));
}

void store_log(unsigned char gear_index,unsigned char* speed)
{
    
    
    get_time();
    
    eeprom_addr = (count % 10) * 10;
    
    write_AT24C04(eeprom_addr++, time[0]); //HH
    __delay_ms(5);
    write_AT24C04(eeprom_addr++, time[1]); 
    __delay_ms(5);
    
    write_AT24C04(eeprom_addr++, time[3]); //MM
    __delay_ms(5);
    write_AT24C04(eeprom_addr++, time[4]); 
    __delay_ms(5);
    
    write_AT24C04(eeprom_addr++, time[6]); //SS
    __delay_ms(5);
    write_AT24C04(eeprom_addr++, time[7]); 
    __delay_ms(5);
    
    write_AT24C04(eeprom_addr++, gear[gear_index][0]); 
    __delay_ms(5);
    write_AT24C04(eeprom_addr++, gear[gear_index][1]); 
    __delay_ms(5);
    //__delay_ms(5);
    
    write_AT24C04(eeprom_addr++, speed[0]); 
    __delay_ms(5);
    write_AT24C04(eeprom_addr++, speed[1]);
    __delay_ms(5);
    
    count++;
    
}
unsigned char* get_speed(void)
{
    // Implement the speed function
    unsigned int adc_value = read_adc(CHANNEL4);
    
    unsigned int speed = ((uint32_t)adc_value * 99U) / 1023U;
    
    //uint8_t data[3];
            
    //data[0] = (speed/10)+'0'; //tens
    //data[1] = (speed%10)+'0'; //ones
    //data[2] = '\0';

    //return speed;
    speed_data[0] = (speed/10)+'0';
    speed_data[1] = (speed%10) + '0';
    speed_data[2] = '\0';
    
    clcd_print(speed_data, LINE2(14));
    return speed_data;
   
}


void get_gear(void)
{
    unsigned char gear_changed = 0;
    if(key == SW1)
    {
        if(gear_index < 5)
        {
            gear_index++;
            gear_changed = 1; 
        }
    }
    else if(key == SW2)
    {
        if(gear_index > 0)
        {
            gear_index--;
            gear_changed = 1;
        }
    }
    else if(key == SW3)
    {
        gear_index = 7;
        gear_changed = 1;
    }        
    else if(key == SW4)
    {
        gear_index = 6;
        gear_changed = 1;
    }
    clcd_print(gear[gear_index], LINE2(10));
    
    if(gear_changed)
    {
        
        store_log(gear_index,speed_data);
    }
    
    
}



void view_dashboard(void) 
{
    clcd_print((unsigned char *)"TIME    EVENT SP", LINE1(0));
    get_time();
    get_speed();
    get_gear(); 
    
}

void display_main_menu()
{
    
    if(key == SW1)
    {
        if(arrow_pos == 0)    //SCROLL UP
        {
            arrow_pos = 1;
        }
        else
        {
            if(menu_pos < 2)
                menu_pos++;
        }
    }
    else if(key == SW2)     //SCROLL DOWN
    {
        if(arrow_pos == 1)
        {
            arrow_pos = 0;
        }
        else
        {
            if(menu_pos > 0)
            {
                menu_pos--;
            }
        }
    }
    clcd_print("                ",LINE1(0));
    clcd_print(menu[menu_pos],LINE1(2));
    clcd_print("                ",LINE2(0));
    clcd_print(menu[menu_pos+1],LINE2(2));
    
    if(arrow_pos == 0)
    {
        clcd_print("  ",LINE1(0));
        clcd_print("->",LINE1(0));
        
    }
    else
    {
        clcd_print("  ",LINE2(0));
        clcd_print("->",LINE2(0));
       
    }
    unsigned char index = menu_pos + arrow_pos;
    
    if(key == SW11)
    {
        switch(index)
        {
            case 0:
                state = e_view_log;
            break;

            case 1:
                state = e_clear_log;
            break;

            case 2:
               state = e_set_time;
            break;

            case 3:
                state = e_download_log;
            break;
            
        }
    }
}
void view_log(void)
{
    unsigned char addr;
    unsigned char log[17];
    
    if(count > 0)
    {
        if(key == SW1)
        {
            if(log_index < 9)
            {
                log_index++;
            }
        }
        if(key == SW2)
        {
            if(log_index > 0)
            {
                log_index--;
            }
        }
        if(key == SW1 || key == SW2)
        {
            addr = log_index * 10;

            log[0] = (log_index % 10) + '0';
            log[1] = ' ';

            log[2] = read_AT24C04(addr++);
            log[3] = read_AT24C04(addr++);
            log[4] = ':';

            log[5] = read_AT24C04(addr++);
            log[6] = read_AT24C04(addr++);
            log[7] = ':';

            log[8] = read_AT24C04(addr++);
            log[9] = read_AT24C04(addr++);
            log[10] = ' ';

            log[11] = read_AT24C04(addr++);
            log[12] = read_AT24C04(addr++);
            log[13] = ' ';

            log[14] = read_AT24C04(addr++);
            log[15] = read_AT24C04(addr++);
            log[16] = '\0';


            clcd_print("                ",LINE1(0));
            clcd_print("LOG DETAILS",LINE1(3));
            clcd_print("                ",LINE2(0));
            clcd_print(log,LINE2(0));
            
        }
    }
    else
    {
        clcd_print("NO LOGS",LINE1(0));
        clcd_print("                ",LINE2(0));
    }
    key = 0;
}
        
void download_log(void)
{
    //clcd_print("                ",LINE1(0));
    //clcd_print("U R IN DOWNLOAD",LINE1(0));
    //clcd_print("                ",LINE2(0));
    unsigned char address;
    unsigned char uart_log[17];
    unsigned char uart_index = 0;
    
    
    if(count > 0)
    {
        //clcd_print("                ",LINE1(0));
        
        puts("S.NO TIME     EVENT  SPEED\r\n");
        while(uart_index < count  && uart_index < 10)
            {
                //uart display of logs

                address = uart_index * 10;


                uart_log[0] = ((count - 1 - uart_index + 10) % 10) + '0';
                uart_log[1] = ' ';

                uart_log[2] = read_AT24C04(address++);
                uart_log[3] = read_AT24C04(address++);
                uart_log[4] = ':';

                uart_log[5] = read_AT24C04(address++);
                uart_log[6] = read_AT24C04(address++);
                uart_log[7] = ':';

                uart_log[8] = read_AT24C04(address++);
                uart_log[9] = read_AT24C04(address++);

                uart_log[10] = ' ';

                uart_log[11] = read_AT24C04(address++);
                uart_log[12] = read_AT24C04(address++);

                uart_log[13] = ' ';

                uart_log[14] = read_AT24C04(address++);
                uart_log[15] = read_AT24C04(address++);

                uart_log[16] = '\0';

                puts(uart_log);
                puts("\r\n");
                uart_index++;

        }
        // still not printing need to solve,,
        clcd_print("                ",LINE1(0));
        clcd_print("DOWNLOADING...",LINE1(0));
        
        clcd_print("                ",LINE2(0));
        
   
    }
    else
    {
        puts("     NO LOGS FOUND   ");
        puts("\n\r");
    }
}
void clear_log(void)
{
    log_index = 0;
    count = 0;
    
    if(log_index == 0)
    {
        clcd_print("                ",LINE1(0));
        clcd_print("LOG IS CLEATRED",LINE2(0));
        clcd_print("                ",LINE2(0));
    }
        
    
   
}
void set_time(void)
{
    //clcd_print("                ",LINE1(0));
    //clcd_print("U R IN SET TIME",LINE1(0));
    //clcd_print("                ",LINE2(0));    
    
    static unsigned char field = 0; // 0-HH, 1-MM, 2-SS
    static unsigned char set_time[3];
    static unsigned char init_flag = 0;
    static unsigned int blink_count = 0;
    static unsigned char blink = 1;

    // Custom circular order: HH ? SS ? MM ? HH
    static const unsigned char next_field[3] = {2, 0, 1};

    // Read RTC only once
    if(init_flag == 0)
    {
        unsigned char hr = read_ds1307(HOUR_ADDR);
        unsigned char min = read_ds1307(MIN_ADDR);
        unsigned char sec = read_ds1307(SEC_ADDR);


        set_time[0] = ((hr >> 4) * 10) + (hr & 0x0F);  // BCD convertion
        set_time[1] = ((min >> 4) * 10) + (min & 0x0F);
        set_time[2] = ((sec >> 4) * 10) + (sec & 0x0F);

        init_flag = 1;
    }

    // Blink logic
    blink_count++;
    if(blink_count > 2000)
    {
        blink = !blink;
        blink_count = 0;
    }

    clcd_print("SET TIME", LINE1(4));

    char display[9];

    // HH
    if(field == 0 && blink == 0)
    {
        display[0] = ' ';
        display[1] = ' ';
    }
    else
    {
        display[0] = (set_time[0] / 10) + '0';
        display[1] = (set_time[0] % 10) + '0';
    }

    display[2] = ':';

    // MM
    if(field == 1 && blink == 0)
    {
        display[3] = ' ';
        display[4] = ' ';
    }
    else
    {
        display[3] = (set_time[1] / 10) + '0';
        display[4] = (set_time[1] % 10) + '0';
    }

    display[5] = ':';

    // SS
    if(field == 2 && blink == 0)
    {
        display[6] = ' ';
        display[7] = ' ';
    }
    else
    {
        display[6] = (set_time[2] / 10) + '0';
        display[7] = (set_time[2] % 10) + '0';
    }

    display[8] = '\0';

    clcd_print(display, LINE2(4));

    //  Increment
    if(key == SW1)
    {
        if(field == 0)
            set_time[0] = (set_time[0] + 1) % 24;
        else if(field == 1)
            set_time[1] = (set_time[1] + 1) % 60;
        else
            set_time[2] = (set_time[2] + 1) % 60;
    }

    //  Decrement
    if(key == SW2)
    {
        if(field == 0)
            set_time[0] = (set_time[0] == 0) ? 23 : set_time[0] - 1;
        else if(field == 1)
            set_time[1] = (set_time[1] == 0) ? 59 : set_time[1] - 1;
        else
            set_time[2] = (set_time[2] == 0) ? 59 : set_time[2] - 1;
    }

    //  Field change (circular custom order)
    if(key == SW3)
    {
        field = next_field[field];
    }

    //  Save and exit
    if(key == SW11)
    {
        write_ds1307(HOUR_ADDR, ((set_time[0] / 10) << 4) | (set_time[0] % 10));
        write_ds1307(MIN_ADDR, ((set_time[1] / 10) << 4) | (set_time[1] % 10));
        write_ds1307(SEC_ADDR, ((set_time[2] / 10) << 4) | (set_time[2] % 10));

        init_flag = 0;
        field = 0;
        state = e_dashboard;
    }

    key = 0;

}

