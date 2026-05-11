#ifndef VS_HOST_GTE_H
#define VS_HOST_GTE_H

#include <stdint.h>

#define VS_HOST_GTE_ONE 4096

typedef struct VsHostMatrix {
    int16_t m[3][3];
    int32_t t[3];
} VsHostMatrix;

typedef struct VsHostVector {
    int32_t vx;
    int32_t vy;
    int32_t vz;
    int32_t pad;
} VsHostVector;

typedef struct VsHostSVector {
    int16_t vx;
    int16_t vy;
    int16_t vz;
    int16_t pad;
} VsHostSVector;

void vs_host_gte_set_rot_matrix(const VsHostMatrix* matrix);
void vs_host_gte_set_trans_matrix(const VsHostMatrix* matrix);
VsHostSVector* vs_host_gte_apply_matrix_sv(const VsHostMatrix* matrix, const VsHostSVector* input, VsHostSVector* output);
VsHostVector* vs_host_gte_apply_matrix(const VsHostMatrix* matrix, const VsHostSVector* input, VsHostVector* output);
VsHostVector* vs_host_gte_apply_matrix_lv(const VsHostMatrix* matrix, const VsHostVector* input, VsHostVector* output);
int32_t vs_host_gte_rsqrt(int32_t value);

#endif
