#ifndef LCD_MENU_SYS_H
#define LCD_MENU_SYS_H

#include <stdint.h>

// Menu Actions
#define DOWN 'd'
#define UP 'u'
#define PRESS 'p'

// Menu item types
#define PREV_SCRN 'p'
#define SUBMENU 's'
#define ENTRY 'e'
#define TOGGLE 't'

#define NULL 0

typedef struct menu_item
{
    const char *text_to_display;
    const char item_type;
    char *value_to_display;
} menu_item;

extern menu_item *main_menu[7];

void menu_action(menu_item *menu, char action);

#endif
