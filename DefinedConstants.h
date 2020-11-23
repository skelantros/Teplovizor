#ifndef DEFINED_CONSTANTS
#define DEFINED_CONSTANTS

#include <Arduino.h>
#include <SPFD5408_Adafruit_TFTLCD.h>

// Calibrates value
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

//These are the pins for the shield!
#define YP A1
#define XM A2
#define YM 7
#define XP 6

// LCD Pin

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4 // Optional : otherwise connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define COLORS_COUNT 0x0008

// Buttons

#define BUTTONS 2 //количество кнопок на экране
#define BUTTON_START 0 // типа id для кнопки, чтобы прорисовывать и использовать в switch 
#define BUTTON_MENU 1

#define MENU_OPTIONS 5
#define MENU_GRID 0
#define MENU_COLOR 1
#define MENU_TIME 2
#define MENU_ANGLE 3
#define MENU_BACK 4

#define GRID_OPTIONS 3
#define GRID_MINUS 0
#define GRID_PLUS 1
#define GRID_BACK 2

#define COLOR_OPTIONS 3
#define COLOR_LEFT 0
#define COLOR_RIGHT 1
#define COLOR_BACK 2

#define TIME_OPTIONS 3
#define TIME_LEFT 0
#define TIME_RIGHT 1
#define TIME_BACK 2

#define ANGLE_OPTIONS 3
#define ANGLE_LEFT 0
#define ANGLE_RIGHT 1
#define ANGLE_BACK 2

#define START_OPTIONS 2
#define BUTTON_BACK 0
#define BUTTON_REFRESH 1

#define PLATFORM_VER_PORT 24
#define PLATFORM_HOR_PORT 22

void initGrayPalette(uint16_t* arr);
void initRolPalette(uint16_t* arr);
void showInitScreen(Adafruit_TFTLCD &tft);
void printArray(double** arr, int rows, int columns);

#endif
