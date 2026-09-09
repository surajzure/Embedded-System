/*
 * File:   main.c
 * Author: suraj
 */

#include <xc.h>
#include "black_box.h"
#include "adc.h"
#include "clcd.h"
#include "i2c.h"
#include "rtc.h"
#include "matrix_keypad.h"
#include "external_eeprom.h"
#include "uart.h"

State_t state;

void init_config()
{
    init_clcd();
    init_adc();
    init_i2c();
    init_ds1307();
    init_matrix_keypad();
    init_ds1307();
    init_uart();
    
    state = e_dashboard;
    
}

//declare the key 
unsigned char key = 0;
static unsigned char toggle_key = 0;
char original_password[9]="11111111";
char user_password[9];
static int index = 0;
static int attempts = 0;
static unsigned char first_entry = 1;

void clear_screen(void)
{
    clcd_print("                ",LINE1(0));
    clcd_print("                ",LINE2(0));
}
void blink_cursor(void)
{
    static unsigned int ccount = 0;

    if(ccount++ == 15000)
    {
        ccount = 0;
        clcd_putch('_', LINE2(index));
    }
    else if(ccount == 7500)
    {
        clcd_putch(' ', LINE2(index));
    }
}

int my_strcmp(char str1[],char str2[])
{
    int i = 0;

    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
            return 1;   // NOT MATCH
        i++;
    }

    // Both must end at same time
    if (str1[i] == '\0' && str2[i] == '\0')
        return 0;       // MATCH
    else
        return 1;       // NOT MATCH
}

//void main(void) 
//{
//    init_config();
//
//    while(1)
//    {
//        // Detect key press
//        key = read_matrix_keypad(EDGE); // make it global
//        
//        if(key == SW12)
//        {
//            return;
//        }
//        //key = read_matrix_keypad(EDGE);
//        if(key == SW10)
//        {
//            toggle_key = !toggle_key;
//            clear_screen();
//            
//            if(toggle_key)
//            {
//                state = e_password;
//                clcd_print("",LINE1(0));
//            }
//            else
//            {
//                state = e_dashboard;
//                clcd_print("DASHBOARD",LINE1(0));
//            }
//            key = 0xFF;
//        }
//        
//        switch (state)
//        {
//            case e_dashboard:
//                // Display dashboard
//                view_dashboard();
//                break;
//            
//            case e_main_menu:
//                // Display dashboard
//                display_main_menu();
//                break;
//            
//            case e_view_log:
//                // Display dashboard
//                view_log();
//                break;
//                 
//            case e_download_log:
//                download_log();
//                break;
//                
//            case e_clear_log:
//                clear_log();
//                break;
//                
//                      
//            case e_set_time:
//                set_time();
//                break;
//                
//        }
//        
//    }
//    
//}










void read_password()
{
    unsigned char key = read_matrix_keypad(EDGE);
    
    if (key != SW1 && key != SW2)
        return;

    
    if(index<8)
    {
        
        if(key == SW1)
            user_password[index] = '0';
        else if(key == SW2)
            user_password[index] = '1';
        else
            return;
        
        clcd_putch('*',LINE2(index));
        index++;
        
        if(index<8)
        {
            clcd_putch('_',LINE2(index));
            clcd_putch(' ',LINE2(index));
        }
    }
}

void main(void) 
{
    init_config();
    
    while(1)
    {
                                    // Detect key press
            key = read_matrix_keypad(EDGE); // make it global

            if(key == SW12)    //exit key
            {
                state = e_main_menu;
                clear_screen();
                key = 0;
            }
            if(key == SW10)    //go in dashboard
            {
                    toggle_key = !toggle_key;
                    clear_screen();

                    if(toggle_key)
                    {
                        state = e_password;
                        clcd_print("PASSWORD SCREEN",LINE1(0));
                    }
                    else
                    {
                        state = e_dashboard;
                        clcd_print("DASHBOARD",LINE1(0));
                    }
                        key = 0xFF;
            }

            switch (state)
            {
                    case e_dashboard:
                    // Display dashboard
                            view_dashboard();
                            break;
                            
                    case e_password:
                        if(first_entry)
                        {
                            clear_screen();
                            clcd_print("ENTER PASSWORD",LINE1(0));
                            clcd_putch('_',LINE2(0));
                            first_entry = 0;
                        }
                        if(index < 8)
                        {
                            blink_cursor();
                            read_password();
                        }
                        else
                        {
                    
                            user_password[8]='\0';
                            if(my_strcmp(original_password,user_password)==0)   //PASSWORD MATCH
                             {
                                      clear_screen();
                                      state = e_main_menu;
                              }
                             else            //PASSWORD WRONG
                             {
                                attempts++;
                                clcd_print("                ", LINE1(0));
                                clcd_print("WRONG PASSWD",LINE1(0));

                                clcd_putch('0' + (5 - attempts), LINE2(0));
                                clcd_print("ATTEMPTS LEFT", LINE2(1));   
                                if(attempts == 5)  //SYSTEM LOCK
                                {
                                     __delay_ms(1500);
                                    clcd_print("                ", LINE1(0));
                                    clcd_print("SYSTEM LOCKED",LINE1(0));
                                    clcd_print("                ", LINE2(0));
                                    clcd_print("RESET BOARD",LINE2(0));
                                    while(1);

                                }
                               __delay_ms(1500);
                                index = 0;     //RESET FOR NEXT TRY
                                clcd_print("                ", LINE1(0));
                                clcd_print("ENTER PASSWORD",LINE1(0));
                                clcd_print("                ",LINE2(0));
                                clcd_putch('_',LINE2(0));
                              }
                           }
                        break;
                    case e_main_menu:
                    // Display dashboard
                        display_main_menu();
                            break;

                    case e_view_log:
                    // Display dashboard
                            view_log();
                            break;

                    case e_download_log:
                            download_log();
                            break;

                    case e_clear_log:
                            clear_log();
                            break;


                    case e_set_time:
                            clear_screen();
                            set_time();
                            break;

            }
            

    }       
        
            
}