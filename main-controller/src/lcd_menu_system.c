#include <msp430fr2153.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_menu_system.h"
#include "my_float.h"
#include <stdbool.h>
#include "RTC.h"
#include "utils.h"

#define MAIN_MENU_SIZE 7
#define RTC_MENU_SIZE 7
#define WS2812B_MENU_SIZE 4

static uint8_t current_row = 0;         // LCD row indicator number (0-3)
static uint8_t current_menu_item = 0;   // The item we're actually on in the list

// Menu hierarchy instances
static menu_item *main_menu[MAIN_MENU_SIZE];
static menu_item *ws2812b_menu[WS2812B_MENU_SIZE];
static menu_item *rtc_menu[RTC_MENU_SIZE];

static menu_sys lcd_menu = {main_menu, MAIN_MENU_SIZE};

// Shared Main Menu Return - all submenus branching off of Main Menu can return here.
static menu_item main_menu_ret = {"Main Menu", PREV_SCRN, NULL, main_menu, MAIN_MENU_SIZE};

// RTC MENU
static menu_item year = {"Year             ", ENTRY, 26, NULL, 0};
static menu_item month = {"Month            ", ENTRY, 04, NULL, 0};
static menu_item date = {"Date             ", ENTRY, 01, NULL, 0};
static menu_item hour = {"Hour             ", ENTRY, 10, NULL, 0};
static menu_item minute = {"Minute           ", ENTRY, 00, NULL, 0};
static menu_item second = {"Second           ", ENTRY, 00, NULL, 0};
static menu_item *rtc_menu[RTC_MENU_SIZE] = {&main_menu_ret, &year, &month,
                                            &date, &hour, &minute, &second};

// WS2812B COLOR MENU
static menu_item red = {"Red             ", ENTRY, 255, NULL, 0};
static menu_item green = {"Green           ", ENTRY, 000, NULL, 0};
static menu_item blue = {"Blue            ", ENTRY, 255, NULL, 0};
static menu_item *ws2812b_menu[WS2812B_MENU_SIZE] = {&main_menu_ret, &red, &green, &blue};

// MAIN MENU
static menu_item main_screen = {"Main Screen", MAIN_SCRN, NULL, NULL, 1};
static menu_item window_size = {"Window Size     ", ENTRY, 5, NULL, 0};
static menu_item lcd_contrast = {"LCD Contrast    ", ENTRY, 50, NULL, 0};
static menu_item rtc_settings = {"RTC Settings", SUBMENU, NULL, rtc_menu, RTC_MENU_SIZE};
static menu_item ws2812b_color = {"WS2812B Color", SUBMENU, NULL, ws2812b_menu, WS2812B_MENU_SIZE};
static menu_item cursor_on_off = {"Cursor    ", TOGGLE, 0, NULL, 0};
static menu_item cursor_blink = {"Blink     ", TOGGLE, 0, NULL, 0};
static menu_item *main_menu[MAIN_MENU_SIZE] = {&main_screen, &window_size, &lcd_contrast, &rtc_settings,
                                               &ws2812b_color, &cursor_on_off, &cursor_blink};

/**
 * Updates the LCD with the current visible state of the menu.
 * - Sets the row indicator based on 'current_row'
 * - Displays the proper number of items above and below the current item
 * - Displays the value associated with the visible items if they have one
 */
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

        // Writing menu item value for TOGGLE and ENTRY item types
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
            // Converts numerical representation of value to ASCII for LCD
            {
            char value[4];
            ui8toa((uint16_t)(lcd_menu.current_submenu)[item]->value_to_display, value);
            LCD_write_string(value);
            break;
            }
        }
    }
}

/**
 * Modifies the contents of the menu hierarchy based on the action
 * and the item type if relevant.
 * @param action - the way in which the menu is being interacted with
 * @param keypad_data - any incoming data from the keypad
 *
 * @return a flag for caller to use:
 * 0: nothing
 * 1: caller should poll keypad for an entry value
 * 2: caller should overwrite LCD with main screen
 */
int menu_action(char action, uint8_t keypad_data)
{
    switch(action)
    {
    // See docs for details on scrolling behavior
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

    // "..."
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

    // The menu item type defines the behavior of the menu
    // when a 'press' input is received.
    case PRESS:
        
        switch((lcd_menu.current_submenu)[current_menu_item]->item_type)
        {
        // Return to the main screen ("out of menu")
        case MAIN_SCRN:
            return 2;
            break;

        // Navigate to super- or submenu using the `menu_link` fields
        case PREV_SCRN:
        case SUBMENU:
            lcd_menu.current_menu_size = (lcd_menu.current_submenu)[current_menu_item]->menu_link_size;
            lcd_menu.current_submenu = (lcd_menu.current_submenu)[current_menu_item]->menu_link;
            current_menu_item = 0;
            current_row = 0;
            menu_ui_update();
            break;

        // Toggles the value of the item. 0 = OFF, 1 = ON.
        case TOGGLE:
            (lcd_menu.current_submenu)[current_menu_item]->value_to_display ^= 1;
            menu_ui_update();
            break;

        // Lets the caller know to start receiving keypad data.
        // Typically followed by another call to this function
        // with action 'KEY_DATA'.
        case ENTRY:
            return 1;
            break;

        case NONE:
            menu_ui_update();
            break;
        }
        break;

    // Modifies the data held by menu items (usually of type ENTRY)
    case KEY_DATA:
        {    
        // Setting max values so there is no overflow on LCD lines. These fields
        // have a width of 3 (i.e., max of 255). 'value_to_display' field maxes
        // out at 255, so no need to cap these ones.
        bool on_red = (lcd_menu.current_submenu)[current_menu_item] == &red;
        bool on_green = (lcd_menu.current_submenu)[current_menu_item] == &green;
        bool on_blue = (lcd_menu.current_submenu)[current_menu_item] == &blue;
        bool on_contrast = (lcd_menu.current_submenu)[current_menu_item] == &lcd_contrast;

        // All others have a width of 2 (i.e., max of 99).
        if(!on_red && !on_green && !on_blue && !on_contrast)
        {
            if(keypad_data > 99)
            {
                keypad_data = 99;
            }
        }
    
        // Lock in value from keypad
        (lcd_menu.current_submenu)[current_menu_item]->value_to_display = keypad_data;
        menu_ui_update();
        break;
        }

    // Enter menu from main screen
    case ENTER:
        current_menu_item = 0;
        current_row = 0;
        lcd_menu.current_submenu = main_menu;
        lcd_menu.current_menu_size = MAIN_MENU_SIZE;
        menu_ui_update();
        break;
    }
    
    return 0;
    
}

/**
 * Returns the current temp. averaging window
 * size value stored in the menu item.
 */
uint8_t menu_get_window_size()
{
    return window_size.value_to_display;
}

/**
 * Sets the time and date values in the RTC menu.
 */
void menu_update_time_data(MCP7940N_time *rtc_time)
{
    hour.value_to_display   = BCDtoDEC(rtc_time->hours & 0x3F);     // Strip 12/24 hr bit
    minute.value_to_display = BCDtoDEC(rtc_time->minutes);
    second.value_to_display = BCDtoDEC(rtc_time->seconds & 0x7F);   // Strip ST bit
    month.value_to_display  = BCDtoDEC(rtc_time->month & 0x1F);     // Strip LPYR bit
    date.value_to_display   = BCDtoDEC(rtc_time->date & 0x3F);
    year.value_to_display   = BCDtoDEC(rtc_time->year);
}
