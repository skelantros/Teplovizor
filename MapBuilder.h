#ifndef MAPBUILDER_HEADER
#define MAPBUILDER_HEADER
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <Arduino.h>
#include "MeasureConfigurator.h"

template <typename T>
struct Matrix {
	T** arr;
	int rows;
	int columns;
};

template <typename T>
struct Array {
    T* arr;
    int size;
};

class ColorPalette {
    Array<uint16_t> palette;
    double min_temp, max_temp;
public:
    ColorPalette();
    ColorPalette(Array<uint16_t> palette, double min_temp, double max_temp);
    uint16_t getColor(double temp);
};

class MapBuilder {
    Adafruit_TFTLCD *tft;
    int rows, columns;
    uint16_t x_begin, y_begin;
    ColorPalette palette;
    Matrix<double> source;

public:
    MapBuilder();
    MapBuilder(Adafruit_TFTLCD *tft, uint16_t x_begin, uint16_t y_begin, 
               int rows, int columns, ColorPalette palette, Matrix<double> source);
    void draw();
};

#endif
