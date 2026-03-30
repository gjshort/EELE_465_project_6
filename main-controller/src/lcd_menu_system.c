#include <msp430fr2153.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_menu_system.h"

static const char *ROW_INDICATOR = ">";

static uint8_t current_row = 0;
static uint8_t current_menu_item = 0;

menu_sys lcd_menu = {main_menu, sizeof(main_menu)/sizeof(menu_item*)};

// MAIN MENU
static menu_item main_screen = {"Main Screen", PREV_SCRN, NULL, };
static menu_item window_size = {"Window Size", ENTRY, "05"};
static menu_item lcd_contrast = {"RTC Settings", SUBMENU, NULL};
static menu_item ws2812b_color = {"WS2812B Color", SUBMENU, NULL};
static menu_item cursor_on_off = {"Cursor On/Off", TOGGLE, NULL};
static menu_item cursor_blink = {"Cursor Blink", TOGGLE, " ON  OFF "};
menu_item *main_menu[MAIN_MENU_SIZE] = {&main_screen, &window_size, &lcd_contrast,
                                        &ws2812b_color, &cursor_on_off, &cursor_blink};


static void menu_ui_update(menu_item **menu)
{
    LCD_clear();
    uint8_t item, row;
    for(item = current_menu_item - current_row, row = 0; item <= current_menu_item - current_row + 3; item++, row++)
    {
        if(row == current_row)
        {
            LCD_set_cursor(0, row);
            LCD_write_string(ROW_INDICATOR);
        }
        LCD_set_cursor(1, row);
        LCD_write_string(menu[item]->text_to_display);
    }
}

void menu_action(menu_item **menu, char action)
{
    switch(action)
    {
    case DOWN:
        // Don't go past the last menu item
        if(current_menu_item < lcd_menu.current_menu_size - 1)
        {
            current_menu_item++;

            if(current_menu_item >= 2)
            {
                // Keep row indicator on row 2 if there're more items below not shown
                if(current_menu_item < lcd_menu.current_menu_size - 1)
                {
                    current_row = 2;
                }
                else 
                {
                    current_row = 3;
                }
                
            }
            else 
            {
                current_row++;
            }
            
            menu_ui_update(menu);
        }
        break;
    case UP:

        // Don't go past first menu item
        if (current_menu_item > 0)
        {
            current_menu_item--;

            if(current_menu_item <= lcd_menu.current_menu_size - 3)
            {
                // Keep row indicator on row 1 if there're more items above not shown
                if(current_menu_item > 0)
                {
                    current_row = 1;
                }
                else 
                {
                    current_row = 0;
                }

            }
            else 
            {
                current_row--;
            }
        
            menu_ui_update(menu);
        }
        break;
    case PRESS:
        if(menu[current_menu_item]->item_type == PREV_SCRN)
        {

        }
        break;
    }
    
}
