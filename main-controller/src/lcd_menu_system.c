#include <msp430fr2153.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_menu_system.h"
#include "my_float.h"

static uint8_t current_row = 0;
static uint8_t current_menu_item = 0;

// Global Menu State
menu_sys lcd_menu = {main_menu, MAIN_MENU_SIZE};

// Shared Main Menu Return
static menu_item main_menu_ret = {"Main Menu", PREV_SCRN, NULL, main_menu, MAIN_MENU_SIZE};

// MAIN MENU
static menu_item main_screen = {"Main Screen", PREV_SCRN, NULL, NULL, 1};
static menu_item window_size = {"Window Size      ", ENTRY, 5, NULL, 0};
static menu_item lcd_contrast = {"LCD Contrast     ", ENTRY, 50, NULL, 0};
static menu_item rtc_settings = {"RTC Settings", SUBMENU, NULL, rtc_menu, RTC_MENU_SIZE};
static menu_item ws2812b_color = {"WS2812B Color", SUBMENU, NULL, ws2812b_menu, WS2812B_MENU_SIZE};
static menu_item cursor_on_off = {"Cursor    ", TOGGLE, 0, NULL, 0};
static menu_item cursor_blink = {"Blink     ", TOGGLE, 0, NULL, 0};
menu_item *main_menu[MAIN_MENU_SIZE] = {&main_screen, &window_size, &lcd_contrast, &rtc_settings,
                                        &ws2812b_color, &cursor_on_off, &cursor_blink};

// RTC MENU
static menu_item year = {"Year             ", ENTRY, 26, NULL, 0};
static menu_item month = {"Month            ", ENTRY, 04, NULL, 0};
static menu_item date = {"Date             ", ENTRY, 01, NULL, 0};
static menu_item hour = {"Hour             ", ENTRY, 10, NULL, 0};
static menu_item minute = {"Minute           ", ENTRY, 00, NULL, 0};
static menu_item second = {"Second           ", ENTRY, 00, NULL, 0};
menu_item *rtc_menu[RTC_MENU_SIZE] = {&main_menu_ret, &year, &month,
                                      &date, &hour, &minute, &second};

// WS2812B COLOR MENU
static menu_item red = {"Red             ", ENTRY, 255, NULL, 0};
static menu_item green = {"Green           ", ENTRY, 000, NULL, 0};
static menu_item blue = {"Blue            ", ENTRY, 255, NULL, 0};
menu_item *ws2812b_menu[WS2812B_MENU_SIZE] = {&main_menu_ret, &red, &green, &blue};

static void menu_ui_update()
{
    const char *ROW_INDICATOR = ">";
    LCD_clear();
    uint8_t item, row;
    for(item = current_menu_item - current_row, row = 0; item <= current_menu_item - current_row + 3; item++, row++)
    {
        // Placing the row indicator on the current row
        if(row == current_row)
        {
            LCD_set_cursor(0, row);
            LCD_write_string(ROW_INDICATOR);
        }
        
        // Writing menu item text
        LCD_set_cursor(1, row);
        LCD_write_string((lcd_menu.current_submenu)[item]->text_to_display);

        // Writing menu item value
        switch((lcd_menu.current_submenu)[item]->item_type)
        {
        case TOGGLE:
            if((lcd_menu.current_submenu)[item]->value_to_display == 1)
            {
                LCD_write_string("[ON] OFF ");
            }
            else
            {
                LCD_write_string(" ON [OFF]");
            }
            break;
        case ENTRY:
            {
            char value[4];
            ui8toa((uint16_t)(lcd_menu.current_submenu)[item]->value_to_display, value);
            LCD_write_string(value);
            break;
            }
        }
    }
}

// return 1 to poll keypad, 0 to not
int menu_action(char action, uint8_t keypad_data)
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
            
            menu_ui_update();
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
        
            menu_ui_update();
        }
        break;
    case PRESS:
        
        switch((lcd_menu.current_submenu)[current_menu_item]->item_type)
        {
        // Navigate to super- or submenu
        case PREV_SCRN:
        case SUBMENU:
            lcd_menu.current_menu_size = (lcd_menu.current_submenu)[current_menu_item]->menu_link_size;
            lcd_menu.current_submenu = (lcd_menu.current_submenu)[current_menu_item]->menu_link;
            current_menu_item = 0;
            current_row = 0;
            menu_ui_update();
            break;
        case TOGGLE:
            (lcd_menu.current_submenu)[current_menu_item]->value_to_display ^= 1;
            menu_ui_update();
            break;
        case ENTRY:
            return 1;
            break;
        case NONE:
            menu_ui_update();
            break;
        }

        break;
    case KEY_DATA:
        // Lock in value from keypad
        (lcd_menu.current_submenu)[current_menu_item]->value_to_display = keypad_data;
        menu_ui_update();
        break;
    }
    
    return 0;
    
}
