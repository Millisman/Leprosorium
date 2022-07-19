#include "LiquidCrystal4b.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include "mcu/pin_macros.h"
#include "mcu/pins.h"

static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;
static uint8_t _numlines;
static uint8_t _numcols;
static uint8_t _r_ofs[4];


void LiquidCrystal_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    DRIVER(PIN_LCD_RS, OUT);
    DRIVER(PIN_LCD_EN, OUT);
    OFF(PIN_LCD_RS);
    OFF(PIN_LCD_EN);
    DRIVER(PIN_LCD_D4, OUT);
    DRIVER(PIN_LCD_D5, OUT);
    DRIVER(PIN_LCD_D6, OUT);
    DRIVER(PIN_LCD_D7, OUT);
    _delay_ms(50);
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    if (lines > 1) _displayfunction |= LCD_2LINE;
    _numcols = cols;
    _numlines = lines;
    LiquidCrystal_setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);
    if ((dotsize != LCD_5x8DOTS) && (lines == 1)) { _displayfunction |= LCD_5x10DOTS; }
    LiquidCrystal_send(0x03, LCD_SEND_4b_INIT); // set 4 bit mode
    _delay_ms(5); // wait min 4.1 ms
    LiquidCrystal_send(0x03, LCD_SEND_4b_INIT);// set 4 bit mode
    _delay_ms(5); // wait min 4.1 ms
    LiquidCrystal_send(0x03, LCD_SEND_4b_INIT);// set 4 bit mode
    _delay_us(150);
    LiquidCrystal_send(0x02, LCD_SEND_4b_INIT);
    LiquidCrystal_send(LCD_FUNCTIONSET | _displayfunction, LCD_SEND_COMMAND);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LiquidCrystal_display();
    LiquidCrystal_clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    LiquidCrystal_send(LCD_ENTRYMODESET | _displaymode, LCD_SEND_COMMAND);
}

void LiquidCrystal_setRowOffsets(uint8_t row0, uint8_t row1, uint8_t row2, uint8_t row3) {
    _r_ofs[0] = row0; _r_ofs[1] = row1; _r_ofs[2] = row2; _r_ofs[3] = row3;
}

void LiquidCrystal_clear() {
    LiquidCrystal_send(LCD_CLEARDISPLAY, LCD_SEND_COMMAND);
}

void LiquidCrystal_home() {
    LiquidCrystal_send(LCD_RETURNHOME, LCD_SEND_COMMAND);
}

void LiquidCrystal_setCursor(uint8_t col, uint8_t row) {
    const size_t max_lines = sizeof( _r_ofs ) / sizeof(*_r_ofs );
    if ( row >= max_lines ) { row = max_lines - 1; }
    if ( row >= _numlines ) { row = _numlines - 1; }
    LiquidCrystal_send(LCD_SETDDRAMADDR | (col + _r_ofs[row]), LCD_SEND_COMMAND);
}

void LiquidCrystal_noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    LiquidCrystal_send(LCD_DISPLAYCONTROL | _displaycontrol, LCD_SEND_COMMAND);
}

void LiquidCrystal_display() {
    _displaycontrol |= LCD_DISPLAYON;
    LiquidCrystal_send(LCD_DISPLAYCONTROL | _displaycontrol, LCD_SEND_COMMAND);
}

void LiquidCrystal_noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    LiquidCrystal_send(LCD_DISPLAYCONTROL | _displaycontrol, LCD_SEND_COMMAND);
}
void LiquidCrystal_cursor() {
    _displaycontrol |= LCD_CURSORON;
    LiquidCrystal_send(LCD_DISPLAYCONTROL | _displaycontrol, LCD_SEND_COMMAND);
}

void LiquidCrystal_noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    LiquidCrystal_send(LCD_DISPLAYCONTROL | _displaycontrol, LCD_SEND_COMMAND);
}

void LiquidCrystal_blink() {
    _displaycontrol |= LCD_BLINKON;
    LiquidCrystal_send(LCD_DISPLAYCONTROL | _displaycontrol, LCD_SEND_COMMAND);
}

void LiquidCrystal_scrollDisplayLeft(void) {
    LiquidCrystal_send(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT, LCD_SEND_COMMAND);
}

void LiquidCrystal_scrollDisplayRight(void) {
    LiquidCrystal_send(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT, LCD_SEND_COMMAND);
}

void LiquidCrystal_leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    LiquidCrystal_send(LCD_ENTRYMODESET | _displaymode, LCD_SEND_COMMAND);
}

void LiquidCrystal_rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    LiquidCrystal_send(LCD_ENTRYMODESET | _displaymode, LCD_SEND_COMMAND);
}

void LiquidCrystal_autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    LiquidCrystal_send(LCD_ENTRYMODESET | _displaymode, LCD_SEND_COMMAND);
}

void LiquidCrystal_noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    LiquidCrystal_send(LCD_ENTRYMODESET | _displaymode, LCD_SEND_COMMAND);
}

void LiquidCrystal_createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // we only have 8 locations
    LiquidCrystal_send(LCD_SETCGRAMADDR | (location << 3), LCD_SEND_COMMAND);
    for (int i=0; i<8; i++) {
        LiquidCrystal_send(charmap[i], LCD_SEND_DATA);
    }
}

void LiquidCrystal_send(const uint8_t data, const LiquidCrystal_send_t mode) {
    if (mode == LCD_SEND_4b_INIT) {
        mcu_lcd_write4b(data);
        OFF(PIN_LCD_EN);
        _delay_us(0.6);
        ON(PIN_LCD_EN);
        _delay_us(0.6);
        OFF(PIN_LCD_EN);
        _delay_us(40);
        return;
    }
    mcu_lcd_insert((mode == LCD_SEND_DATA) , data);
}
