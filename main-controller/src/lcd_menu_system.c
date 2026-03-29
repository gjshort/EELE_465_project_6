#include <msp430fr2153.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_menu_system.h"

static const char *ROW_INDICATOR = ">";

static uint8_t current_menu_item = 0;

// MAIN MENU
static menu_item main_screen = {"Main Screen", PREV_SCRN, NULL, };
static menu_item window_size = {"Window Size", ENTRY, "05"};
static menu_item lcd_contrast = {"RTC Settings", SUBMENU, NULL};
static menu_item ws2812b_color = {"WS2812B Color", SUBMENU, NULL};
static menu_item cursor_on_off = {"Cursor On/Off", TOGGLE, NULL};
static menu_item cursor_blink = {"Cursor Blink", TOGGLE, " ON  OFF "};
menu_item *main_menu[7] = {&main_screen, &window_size, &lcd_contrast,
                           &ws2812b_color, &cursor_on_off, &cursor_blink};

static void menu_ui_update(menu_item *menu[])
{
    LCD_clear();
    int i;
    for(i = 0; i < 4; i++)
    {
        if(i == current_menu_item)
        {
            LCD_set_cursor(0, i);
            LCD_write_string(ROW_INDICATOR);
        }
        LCD_set_cursor(1, i);
        LCD_write_string(main_menu[i]->text_to_display);
    }
}

void menu_action(menu_item *menu[], char action)
{
    if(action == DOWN)
    {
        if(current_menu_item < 3)
        {
            current_menu_item++;
            menu_ui_update(menu);
        }
    }
    else if (action == UP)
    {
        if(current_menu_item > 0)
        {
            current_menu_item--;
            menu_ui_update(menu);
        }
    }
    else if (action == PRESS)
    {
        if(menu[current_menu_item]->item_type == PREV_SCRN)
        {

        }
    }
}
