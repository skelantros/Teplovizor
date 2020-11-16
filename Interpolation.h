#ifndef INTERPOLATION_HEADER
#define INTERPOLATION_HEADER

double** interpolateArray(double** src, int src_r, int src_c, int dest_r, int dest_c);
void staticInterpolate(uint16_t** src, uint16_t src_r, uint16_t src_c, uint16_t** dest, uint16_t dest_r, uint16_t dest_c);

namespace interpolations_constants {
    const uint16_t res_rows = 240;
    const uint16_t res_columns = 320;
}

#endif