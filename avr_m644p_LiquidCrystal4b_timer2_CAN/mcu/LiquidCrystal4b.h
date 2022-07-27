#pragma once

#include "mcu.h"

// commands
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT      0x00
#define LCD_ENTRYLEFT       0x02
#define LCD_ENTRYSHIFTINCREMENT     0x01
#define LCD_ENTRYSHIFTDECREMENT     0x00

// flags for display on/off control
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON    0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON     0x01
#define LCD_BLINKOFF    0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE     0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

// flags for function set
#define LCD_4BITMODE    0x00
#define LCD_2LINE       0x08
#define LCD_1LINE       0x00
#define LCD_5x10DOTS    0x04
#define LCD_5x8DOTS     0x00

void LiquidCrystal_begin(uint8_t cols, uint8_t rows, uint8_t charsize);
void LiquidCrystal_clear();
void LiquidCrystal_home();
void LiquidCrystal_noDisplay();
void LiquidCrystal_display();
void LiquidCrystal_noBlink();
void LiquidCrystal_blink();
void LiquidCrystal_noCursor();
void LiquidCrystal_cursor();
void LiquidCrystal_scrollDisplayLeft();
void LiquidCrystal_scrollDisplayRight();
void LiquidCrystal_leftToRight();
void LiquidCrystal_rightToLeft();
void LiquidCrystal_autoscroll();
void LiquidCrystal_noAutoscroll();
void LiquidCrystal_setRowOffsets(uint8_t row1, uint8_t row2, uint8_t row3, uint8_t row4);
void LiquidCrystal_createChar(uint8_t, uint8_t[]);
void LiquidCrystal_setCursor(uint8_t, uint8_t);

typedef enum: uint8_t {
    LCD_SEND_COMMAND = 0,
    LCD_SEND_DATA    = 1,
    LCD_SEND_4b_INIT = 2
} LiquidCrystal_send_t;

void LiquidCrystal_send(const uint8_t data, const LiquidCrystal_send_t mode);
