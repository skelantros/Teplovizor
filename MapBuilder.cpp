#include "MapBuilder.h"

double newInterpolateFunction(double arg, int arg1, int arg2, double func1, double func2) {
  if (func1 == func2) return func2;
  return func1 + 1.0 * (arg - arg1) * (func2 - func1) / (arg2 - arg1);
}

MapBuilder::MapBuilder() {}
MapBuilder::MapBuilder(Adafruit_TFTLCD *tft, uint16_t x_begin, uint16_t y_begin, 
               int rows, int columns, ColorPalette palette, Matrix<double> source) {
    this->tft = tft;
    this->x_begin = x_begin;
    this->y_begin = y_begin;
    this->rows = rows;
    this->columns = columns;
    this->source = source;
    this->palette = palette;
}

void MapBuilder::draw() {
    double c_coeff = 1.0 * (source.columns - 1) / (columns - 1);
	double r_coeff = 1.0 * (source.rows - 1) / (rows - 1);

	for (int r = 0; r < rows - 1; ++r) {
		double r_tr = r_coeff * r;
		int r_rnd = floor(r_tr);

		for (int c = 0; c < columns - 1; ++c) {
			double c_tr = c_coeff * c;
			int c_rnd = floor(c_tr);

			double r1 = newInterpolateFunction(r_tr, r_rnd, r_rnd + 1, source.arr[r_rnd][c_rnd], source.arr[r_rnd + 1][c_rnd]);
			double r2 = newInterpolateFunction(r_tr, r_rnd, r_rnd + 1, source.arr[r_rnd][c_rnd + 1], source.arr[r_rnd + 1][c_rnd + 1]);

			double result = newInterpolateFunction(c_tr, c_rnd, c_rnd + 1, r1, r2);
			tft->fillRect(c, r, 1, 1, palette.getColor(result));
		}
	}

	int c_max = source.columns - 1;
	for (int r = 0; r < rows - 1; ++r) {
		double r_tr = r_coeff * r;
		int r_rnd = floor(r_tr);
		double result = newInterpolateFunction(r_tr, r_rnd, r_rnd + 1, source.arr[r_rnd][c_max], source.arr[r_rnd + 1][c_max]);
        tft->fillRect(columns - 1, r, 1, 1, palette.getColor(result));
	}

	int r_max = source.rows - 1;
	for (int c = 0; c < columns - 1; ++c) {
		double c_tr = c_coeff * c;
		int c_rnd = floor(c_tr);
		double result = newInterpolateFunction(c_tr, c_rnd, c_rnd + 1, source.arr[r_max][c_rnd], source.arr[r_max][c_rnd + 1]);
        tft->fillRect(c, rows - 1, 1, 1, palette.getColor(result));
	}
}

ColorPalette::ColorPalette() {}
ColorPalette::ColorPalette(Array<uint16_t> palette, double min_temp, double max_temp) {
    this->palette = palette;
    this->min_temp = min_temp;
    this->max_temp = max_temp;
}

uint16_t ColorPalette::getColor(double temp) {
    return palette.arr[int(palette.size * (temp - min_temp) / (max_temp - min_temp))];
}

void ColorPalette::setMaxTemp(double temp) { max_temp = temp; }
void ColorPalette::setMinTemp(double temp) { min_temp = temp; }