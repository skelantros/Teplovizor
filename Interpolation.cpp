#include "Interpolation.h"
#include <math.h>


double** makeArray(int rows, int columns) {
	double** arr = new double* [rows];
	for (int r = 0; r < rows; ++r) {
		arr[r] = new double[columns];
	}
	return arr;
}

double interpolateFunction(double arg, int arg1, int arg2,
	double func1, double func2) {
	if (func1 == func2) return func1;
	else return (func1 + (func2 - func1) * (arg - arg1)) / (arg2 - arg1);
}

double** resizeHorizontally(double** src, int src_r, int src_c, int dest_c) {
	double** dest = makeArray(src_r, dest_c);
	double coeff = 1.0 * (src_c - 1) / (dest_c - 1);

	for (int c = 0; c < dest_c - 1; ++c) {
		double c_tr = coeff * c;
		int c_rnd = floor(c_tr);
		for (int r = 0; r < src_r; ++r) {
			if (r == src_r - 1 && c == dest_c - 1) continue;
			dest[r][c] = interpolateFunction(c_tr, c_rnd, c_rnd + 1, src[r][c_rnd], src[r][c_rnd + 1]);
		}
	}

	// fill the last column
	for (int r = 0; r < src_r; ++r) {
		dest[r][dest_c - 1] = src[r][src_c - 1];
	}
	return dest;
}

double** resizeVertically(double** src, int src_r, int src_c, int dest_r) {
	double** dest = makeArray(dest_r, src_c);
	double coeff = 1.0 * (src_r - 1) / (dest_r - 1);

	for (int r = 0; r < dest_r - 1; ++r) {
		double r_tr = coeff * r;
		int r_rnd = floor(r_tr);
		for (int c = 0; c < src_c; ++c) {
			if (r == dest_r - 1 && c == src_c - 1) continue;
			dest[r][c] = interpolateFunction(r_tr, r_rnd, r_rnd + 1, src[r_rnd][c], src[r_rnd + 1][c]);
		}
	}

	// fill the last row

	for (int c = 0; c < src_c; ++c) {
		dest[dest_r - 1][c] = src[src_r - 1][c];
	}
	return dest;
}

double** interpolateArray(double** src, int src_r, int src_c, int dest_r, int dest_c) {

	double** hor_resized = resizeHorizontally(src, src_r, src_c, dest_c);
	double** ver_resized = resizeVertically(hor_resized, src_r, dest_c, dest_r);

	for (int i = 0; i < src_r; ++i) {
		delete[] hor_resized[i];
	}
	delete[] hor_resized;

	return ver_resized;
}

void staticHorizontally(byte** src, int src_r, int src_c,
	byte dest[][interpolations_constants::res_columns], int dest_c) {
	double coeff = 1.0 * (src_c - 1) / (dest_c - 1);

	for (int c = 0; c < dest_c - 1; ++c) {
		double c_tr = coeff * c;
		int c_rnd = floor(c_tr);
		for (int r = 0; r < src_r; ++r) {
			if (r == src_r - 1 && c == dest_c - 1) continue;
			dest[r][c] = interpolateFunction(c_tr, c_rnd, c_rnd + 1, src[r][c_rnd], src[r][c_rnd + 1]);
		}
	}

	// fill the last column
	for (int r = 0; r < src_r; ++r) {
		dest[r][dest_c - 1] = src[r][src_c - 1];
	}
}

void staticVertically(byte src[][interpolations_constants::res_columns], int src_r, int src_c,
	byte dest[][interpolations_constants::res_columns], int dest_r) {
	double coeff = 1.0 * (src_r - 1) / (dest_r - 1);

	for (int r = 0; r < dest_r - 1; ++r) {
		double r_tr = coeff * r;
		int r_rnd = floor(r_tr);
		for (int c = 0; c < src_c; ++c) {
			if (r == dest_r - 1 && c == src_c - 1) continue;
			dest[r][c] = interpolateFunction(r_tr, r_rnd, r_rnd + 1, src[r_rnd][c], src[r_rnd + 1][c]);
		}
	}

	// fill the last row
	for (int c = 0; c < src_c; ++c) {
		dest[dest_r - 1][c] = src[src_r - 1][c];
	}
}

byte intermediate_arr[interpolations_constants::res_rows][interpolations_constants::res_columns];


void staticInterpolate(byte** src, int src_r, int src_c, byte dest[][interpolations_constants::res_columns]) {
	if (src_r > interpolations_constants::res_rows || src_c > interpolations_constants::res_columns)
		throw "Source array must be less or equal than result in both dimensions.";
	staticHorizontally(src, src_r, src_c, intermediate_arr, interpolations_constants::res_columns);
	staticVertically(intermediate_arr, src_r, interpolations_constants::res_columns,
		dest, interpolations_constants::res_rows);
}