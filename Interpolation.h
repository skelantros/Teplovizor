#ifndef INTERPOLATION_HEADER
#define INTERPOLATION_HEADER

typedef short byte;

namespace interpolations_constants {
	const byte res_rows = 20;
	const byte res_columns = 50;
}

void staticInterpolate(byte** src, int src_r, int src_c, byte dest[][interpolations_constants::res_columns]);
double** interpolateArray(double** src, int src_r, int src_c, int dest_r, int dest_c);

#endif