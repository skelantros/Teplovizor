#include "MapBuilder.h"

MapBuilder::MapBuilder() {}
MapBuilder::MapBuilder(Adafruit_TFTLCD _tft, uint16_t x_begin, uint16_t y_begin, 
               int rows, int columns, ColorPalette palette, Matrix<double> source) : tft(_tft) {
    //this->tft = tft;
    this->x_begin = x_begin;
    this->y_begin = y_begin;
    this->rows = rows;
    this->columns = columns;
    this->source = source;
    this->palette = palette;
}

void MapBuilder::draw() {}

ColorPalette::ColorPalette() {}
ColorPalette::ColorPalette(Array<uint16_t> palette, double min_temp, double max_temp) {
    this->palette = palette;
    this->min_temp = min_temp;
    this->max_temp = max_temp;
}

uint16_t ColorPalette::getColor(double temp) {
    return palette.arr[int(palette.size * (temp - min_temp) / (max_temp - min_temp))];
}
