#include "DefinedConstants.h"

void initGrayPalette(uint16_t* arr) {
    int red = 0;
    int blue = 0;
    int green = 0;
    for (int i = 0; i < 32; ++i) {
        arr[i] = (red << 11) + (green << 5) + blue;
        red++;
        blue++;
        green += 2;
    }
}

void initRolPalette(uint16_t* arr) {
    int red = 31;
    int blue = 31;
    int green = 0;
  
    for (int i = 0; i < 32;++i){
        if (blue > 0){
            arr[i] = (red << 11) + (green << 5) + blue;
        }else{
            arr[i] = (red << 11) + (green << 5);
        }
        green += 2;
        blue -= 8;
    }
}

void showInitScreen(Adafruit_TFTLCD &tft) {
    tft.setCursor (30, 50);
    tft.setTextSize (3);
    tft.setTextColor(BLACK);
    tft.println("Welcome To");
    tft.setCursor (40, 85);
    tft.println("Main Menu");
    tft.setCursor (55, 150);
    tft.setTextSize (2);
    tft.setTextColor(BLACK);
    tft.println("Teplovizor");

    tft.setCursor (70, 250);
    tft.setTextSize (1);
    tft.setTextColor(BLACK);
    tft.println("Touch to proceed");
}

void printArray(double** arr, int rows, int columns) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            Serial.print(arr[r][c]); Serial.print(' ');
        }
        Serial.println();
    }
}