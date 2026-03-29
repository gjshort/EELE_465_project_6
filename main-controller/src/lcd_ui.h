#ifndef LCD_UI
#define LCD_UI

#include "RTC.h"

void lcd_ui_write_pattern(uint8_t pattern_num);
void lcd_ui_write_period(float pattern_period);
void lcd_ui_write_temp(float temperature);
void lcd_ui_write_date_time(MCP7940N_time *rtc_time);
void lcd_ui_write_window(uint8_t window_size);

#endif
