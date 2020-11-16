#ifndef INTERPOLATION_HEADER
#define INTERPOLATION_HEADER

typedef short byte;

namespace interpolations_constants {
    const uint16_t res_rows = 240;
    const uint16_t res_columns = 320;
}

void staticInterpolate(byte** src, int src_r, int src_c, byte dest[][interpolations_constants::res_columns]);
double** interpolateArray(double** src, int src_r, int src_c, int dest_r, int dest_c);

#endif