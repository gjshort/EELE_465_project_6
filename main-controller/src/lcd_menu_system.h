#ifndef LCD_MENU_SYS_H
#define LCD_MENU_SYS_H

#include <stdint.h>
#include "RTC.h"

// Menu Actions
#define DOWN 'd'        // Scroll Down
#define UP 'u'          // Scroll Up
#define PRESS 'p'       // Click on the menu item
#define NONE 'n'        // Used for updating the display externally with no action
#define KEY_DATA 'k'    // Update menu data with values given by keypad
#define ENTER 'e'       // Enter the menu system from the main screen

// Menu item types
#define MAIN_SCRN 'm'   // The main screen (not really a menu item)
#define PREV_SCRN 'p'   // Changes the current menu to the previous/super menu
#define SUBMENU 's'
#define ENTRY 'e'       // Read/write a value from/to one of these items (ex: the RTC's month)
#define TOGGLE 't'      // Toggle one of these items ON or OFF

#define NULL 0

/**
 * Defines an item that will be added to a list in order to form a menu
 */
typedef struct menu_item
{
    // The text on the LHS of the screen
    const char *text_to_display;
    
    // Defines the behavior of the item during interaction
    const char item_type;

    // The data associated with the menu item
    uint8_t value_to_display;

    // Double pointer to another menu item.
    // Mainly used to link together sub- and supermenus by pointing
    // to the first item in a menu.
    struct menu_item **menu_link;

    // Size of the menu being linked by the pointer above.
    // Easily allows the system to bound scrolling when switching
    // to a sub- or supermenu.
    uint8_t menu_link_size;
} menu_item;

/**
 * Holds the data of the currently active menu. When switching
 * to sub- or supermenus, the instance of this struct will be updated.
 * Allows for generic display and traversal of a menu hierarchy
 * as long as the sub- and supermenus are properly linked.
 */
typedef struct menu_sys
{
    menu_item **current_submenu;
    uint8_t current_menu_size;
} menu_sys;

int menu_action(char action, uint8_t keypad_data);
uint8_t menu_get_window_size();
void menu_update_time_data(MCP7940N_time *rtc_time);

#endif
