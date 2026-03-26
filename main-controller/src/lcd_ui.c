#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lcd_ui.h"
#include "LCD_Driver.h"
#include "my_float.h"
#include "RTC.h"

// Field Offset Coordinates
// See LCD Datasheet and LCD UI Plan doc


// Field Widths
static const uint8_t PATTERN_FIELD_WIDTH = 12;
static const uint8_t PERIOD_FIELD_WIDTH = 15;
static const uint8_t TEMP_FIELD_WIDTH = 7;
static const uint8_t WINDOW_FIELD_WIDTH = 6;
static const uint8_t TIME_DATE_FIELD_WIDTH = 17;

// LED Pattern Names
static char pattern_names[][PATTERN_FIELD_WIDTH + 1] = {"Toggle      ", "Up Counter  ", "In and Out  ", 
                                                        "Down Counter", "Rotate Left ", "Fill Right  "};

/**
 * Writes the name of the active pattern to the LCD
 * @param pattern_num - the number of the current pattern
 * 
 * Sets the cursor to the Pattern Field Offset on the LCD
 * and then writes the active pattern string. The strings
 * are of fixed width so that shorter words fully overwrite
 * previously written longer ones.
 *
 */
void lcd_ui_write_pattern(uint8_t pattern_num)
{
    // UI Formatting
    const uint8_t PATTERN_OFFSET_COL = 0;
    const uint8_t PATTERN_OFFSET_ROW = 0;
    
    LCD_write_string_here(PATTERN_OFFSET_COL, PATTERN_OFFSET_ROW, pattern_names[pattern_num]);   
}

/**
 * Writes the period of the active pattern to the LCD
 * @param pattern_period - floating point rep. of period in seconds
 *
 * Sends formatted strings along with the ASCII rep. of the
 * floating point period.
 *
 */
void lcd_ui_write_period(float pattern_period)
{
    // UI formatting
    const char *period_str = "Period = ";
    const char *seconds_str = " s";
    const uint8_t PERIOD_OFFSET_COL = 0;
    const uint8_t PERIOD_OFFSET_ROW = 1;

    // Convert period to string
    char pat_period_str[16];
    ftoa_2(pattern_period, pat_period_str);

    LCD_set_cursor(PERIOD_OFFSET_COL, PERIOD_OFFSET_ROW);
    LCD_write_string(period_str);
    LCD_write_string(pat_period_str);
    LCD_write_string(seconds_str);
    
}

/**
 * Writes the current temperature to the LCD
 * @param temperature - floating point rep. of temperature in *C
 *
 * Sends formatted strings and the ASCII rep. of the 
 * floating point temperature.
 *
 */
void lcd_ui_write_temp(float temperature)
{
    // UI Formatting
    const uint8_t TEMPERATURE_OFFSET_COL = 0;
    const uint8_t TEMPERATURE_OFFSET_ROW = 2;
    const char* degrees_str = " \xDF";      // Degree sign is in LCD DDRAM @ 0xDF
    const char* celsius_str = "C";

    // Convert temperature to string
    char temp_str[16];
    ftoa_2(temperature, temp_str);

    LCD_set_cursor(TEMPERATURE_OFFSET_COL, TEMPERATURE_OFFSET_ROW);
    LCD_write_string(temp_str);
    LCD_write_string(degrees_str);
    LCD_write_string(celsius_str);

}

/**
 * Pack the given buffer with the RTC time in ASCII
 * @param rtc_time - pointer to a defined struct for the RTC
 * @param buf - buffer the time message will be written to.
 *              MUST BE OF LENGTH 18 OR MORE. USER BEWARE.
 *
 * Helper function for converting RTC time to a string that
 * can be sent to the LCD.
 *
 * Format: "HH:MM:SS MM/DD/YY\0"
 *
 */
static void rtc_time_to_str(MCP7940N_time *rtc_time, char *buf)
 {
    buf[0]  = ((rtc_time->hours & 0x30) >> 4) + '0';      // strip 12/24hr mode bit
    buf[1]  = (rtc_time->hours & 0x0F) + '0';
    buf[2]  = ':';
    buf[3]  = ((rtc_time->minutes & 0xF0) >> 4) + '0';
    buf[4]  = (rtc_time->minutes & 0x0F) + '0';
    buf[5]  = ':';
    buf[6]  = ((rtc_time->seconds & 0x70) >> 4) + '0';    // Strip ST bit
    buf[7]  = (rtc_time->seconds & 0x0F) + '0';
    buf[8] = ' ';
    buf[9] = ((rtc_time->month & 0x10) >> 4) + '0';      // Strip LPYR bit
    buf[10] = (rtc_time->month & 0x0F) + '0';
    buf[11] = '/';
    buf[12] = ((rtc_time->date & 0x30) >> 4) + '0';
    buf[13] = (rtc_time->date & 0x0F) + '0';
    buf[14] = '/';
    buf[15] = ((rtc_time->year & 0xF0) >> 4) + '0';
    buf[16] = (rtc_time->year & 0x0F) + '0';
    buf[17] = '\0';
 }

/**
 * Writes the current time and date to the LCD
 * @param rtc_time - pointer to a defined struct for the RTC
 *
 * Converts the RTC time and date into a formatted string and
 * then writes it to the LCD. 
 */
void lcd_ui_write_date_time(MCP7940N_time *rtc_time)
{
    // UI Formatting
    const uint8_t TIME_DATE_OFFSET_COL = 0;
    const uint8_t TIME_DATE_OFFSET_ROW = 3;

    // Convert RTC data to a string
    char time_date_str[18];
    rtc_time_to_str(rtc_time, time_date_str);

    LCD_write_string_here(TIME_DATE_OFFSET_COL, TIME_DATE_OFFSET_ROW, time_date_str);

}

/**
 * Writes the current temperature averaging window to the LCD
 * @param window_size - the number of past temps being averaged over
 */
void lcd_ui_write_window(uint8_t window_size)
{
    // UI Formatting
    const uint8_t WINDOW_OFFSET_COL = 14;
    const uint8_t WINDOW_OFFSET_ROW = 2;
    const char *n_str = "N = ";

    // Convert window_size to string
    char win_size_str[16];
    ui8toa(window_size, win_size_str);

    LCD_set_cursor(WINDOW_OFFSET_COL, WINDOW_OFFSET_ROW);
    LCD_write_string(n_str);
    LCD_write_string(win_size_str);
}
