#include "common.h"
#include "146C.h"
#include "58578.h"
#include "../../SLUS_010.40/main.h"
#include <stddef.h>

#if defined(PERMUTER) || defined(OBJDIFF)
#define VS_ABS(value) ((value) < 0 ? -(value) : (value))
#endif

typedef struct {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    short unk20;
    short unk22;
    int unk24;
    short unk28;
    short unk2A;
    char unk2C;
    char unk2D;
    char unk2E;
    char unk2F;
    char unk30;
    char unk31;
    char unk32;
    char unk33;
    int unk34;
    int unk38;
    short unk3C;
    short unk3E;
    int unk40[0x254];
    struct {
        char unk0;
        char unk1;
        char unk2;
        char unk3;
        char unk4;
        char unk5;
        char unk6;
        char unk7;
        char unk8;
        char unk9;
        char unkA;
        char unkB;
        char unkC;
        char unkD;
        char unkE;
        char unkF;
        char unk10;
        char unk11;
        char unk12;
        char unk13;
    } unk990[24];
} D_800EB9B8_t;

extern D_800EB9B8_t* D_800EB9B8;

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C0D78);

typedef struct {
    short unk0;
    short unk2;
    char unk4[4];
    u_short unk8[4];
} func_800C0FA8_t;

typedef struct {
    SVECTOR unk0;
    int unk8;
    int unkC;
    short unk10[4];
    short unk18[4];
} func_800C0FA8_t2;

void func_800C0FA8(func_800C0FA8_t* arg0, func_800C0FA8_t2* arg1, MATRIX* arg2)
{
    int temp_v0;
    int i;
    char* new_var;

    for (i = 0; i < 3; ++i) {
        arg1->unk10[i] = arg0->unk8[i];
        arg1->unk18[i] = 0x8000 / *(new_var = &arg0->unk4[i]);
    }

    arg1->unk0.vx = arg0->unk4[3] * 0x10;
    arg1->unk0.vy = -arg0->unk2;
    arg1->unk0.vz = 0;

    RotMatrix_gte(&arg1->unk0, arg2);
}

int func_800C1034(func_800C0FA8_t* arg0, u_short* arg1);
int func_800C110C(func_800C0FA8_t* arg0, u_short* arg1, int arg2);
int func_800C123C(func_800C0FA8_t* arg0, u_short* arg1, int arg2);
int func_800C1384(func_800C0FA8_t* arg0, u_short* arg1, int arg2);
int func_800C1564(void* arg0, void* arg1);

#if defined(PERMUTER) || defined(OBJDIFF)
int func_800C1034(func_800C0FA8_t* arg0, u_short* arg1)
{
    MATRIX matrix;
    func_800C0FA8_t2 work;
    SVECTOR transformed;
    short* delta;
    short* components;
    int i;
    int scaled;
    int sum;

    func_800C0FA8(arg0, &work, &matrix);

    delta = &work.unk0.vx;
    for (i = 0; i < 3; ++i) {
        delta[i] = arg1[i] - work.unk10[i];
    }

    ApplyMatrixSV(&matrix, &work.unk0, &transformed);

    sum = 0;
    components = &transformed.vx;
    for (i = 0; i < 3; ++i) {
        scaled = (components[i] * work.unk18[i]) >> 12;
        sum += scaled * scaled;
    }

    return (sum >> 16) == 0;
}
#else
INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C1034);
#endif

#if defined(PERMUTER) || defined(OBJDIFF)
int func_800C110C(func_800C0FA8_t* arg0, u_short* arg1, int arg2)
{
    MATRIX matrix;
    func_800C0FA8_t2 work;
    SVECTOR transformed;
    short* delta;
    short* components;
    int i;
    int scaled;
    int sum;
    int limit;

    func_800C0FA8(arg0, &work, &matrix);

    delta = &work.unk0.vx;
    for (i = 0; i < 3; ++i) {
        delta[i] = arg1[i] - work.unk10[i];
    }

    ApplyMatrixSV(&matrix, &work.unk0, &transformed);
    if (arg2 != 0) {
        transformed.vy -= arg0->unk4[1] << 4;
    }

    limit = arg0->unk4[1] << 5;
    if (limit < VS_ABS(transformed.vy)) {
        return 0;
    }

    sum = 0;
    components = &transformed.vx;
    for (i = 0; i < 3; ++i) {
        scaled = (components[i] * work.unk18[i]) >> 12;
        sum += scaled * scaled;
    }

    return (sum >> 16) == 0;
}
#else
INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C110C);
#endif

#if defined(PERMUTER) || defined(OBJDIFF)
int func_800C123C(func_800C0FA8_t* arg0, u_short* arg1, int arg2)
{
    MATRIX matrix;
    func_800C0FA8_t2 work;
    SVECTOR transformed;
    short* delta;
    int i;
    int vertical;
    int radius;
    int x;
    int z;
    int threshold;

    func_800C0FA8(arg0, &work, &matrix);

    delta = &work.unk0.vx;
    for (i = 0; i < 3; ++i) {
        delta[i] = arg1[i] - work.unk10[i];
    }

    ApplyMatrixSV(&matrix, &work.unk0, &transformed);
    if (arg2 != 0) {
        transformed.vy -= arg0->unk4[1] << 5;
    }

    radius = arg0->unk4[1];
    if ((radius << 5) < VS_ABS(transformed.vy)) {
        return 0;
    }

    vertical = -((transformed.vy << 3) / radius);
    x = (transformed.vx * work.unk18[0]) >> 12;
    z = (transformed.vz * work.unk18[2]) >> 12;
    threshold = 0x100 - vertical;

    return ((x * x) + (z * z)) < (threshold * threshold);
}
#else
INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C123C);
#endif

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C1384);

#if defined(PERMUTER) || defined(OBJDIFF)
int func_800C1564(void* arg0_raw, void* arg1_raw)
{
    func_800C0FA8_t* arg0;
    u_short* arg1;
    int saved;
    int result;

    arg0 = arg0_raw;
    arg1 = arg1_raw;
    saved = *(int*)arg0->unk4;
    result = 0;

    switch (arg0->unk0) {
    case 1:
        result = func_800C1034(arg0, arg1);
        break;
    case 2:
        arg0->unk4[1] <<= 1;
        result = func_800C110C(arg0, arg1, 1);
        break;
    case 3:
        result = func_800C110C(arg0, arg1, 0);
        break;
    case 4:
        result = func_800C123C(arg0, arg1, 0);
        break;
    case 5:
        arg0->unk4[3] = (saved >> 24) + 0x80;
        result = func_800C123C(arg0, arg1, 1);
        break;
    case 6:
        result = func_800C1384(arg0, arg1, 1);
        break;
    case 7:
        result = func_800C1384(arg0, arg1, 0);
        break;
    }

    *(int*)arg0->unk4 = saved;
    return result;
}
#else
INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C1564);
#endif

void func_800C58F8(int); /* extern */

void func_800C1664(int arg0, int arg1, int arg2)
{
    int var_a0;
    int var_v0;

    var_a0 = arg0;
    if (D_800EB9B8 != NULL) {
        if (var_a0 >= 0x19) {
            var_a0 = 0x18;
        }
        D_800EB9B8->unk2C = var_a0;
        D_800EB9B8->unk40[0] = arg1;
        if (D_800EB9B8->unk2D >= var_a0) {
            var_v0 = 0;
            if (var_a0 == 0) {
                var_v0 = 0xFF;
            }
            D_800EB9B8->unk2D = var_v0;
        }
        D_800EB9B8->unk31 = arg2;
        func_800C58F8(0);
    }
}

void func_800C16DC(void)
{
    if (D_800EB9B8 != NULL) {
        D_800EB9B8->unk2C = 0;
    }
}

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C16FC);

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C1A40);

int func_800C1D84(void)
{
    int i;
    for (i = 0; i < 24; ++i) {
        if (D_800EB9B8->unk990[i].unk0 > 1) {
            return 0;
        }
    }
    return 1;
}

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C1DC4);

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C20B4);

#if defined(PERMUTER) || defined(OBJDIFF)
int func_800C2254(int angle, int index)
{
    short* scratch;
    short* dest;
    int row;
    int component;
    int cosine;
    int sine;
    int value;

    scratch = (short*)0x1F800398;
    dest = (short*)((char*)D_800EB9B8 + 0x48 + (index * 8));

    for (row = 0; row < 0x21; ++row) {
        short* src = scratch;
        short* out = dest;
        for (component = 0; component < 3; ++component) {
            cosine = rcos(angle);
            sine = rsin(angle);
            value = src[0];
            value += (src[3] * cosine) >> 12;
            value += (src[6] * sine) >> 12;
            out[0] = value;
            src++;
            out++;
        }
        dest[3] = row != 0;
        ++index;
        angle += 0x80;
        dest += 4;
    }

    return index;
}
#else
INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C2254);
#endif

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C2368);

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C253C);

void* func_800C282C(void)
{
    _sphericalCamera camera;
    int temp_a0;
    int temp_s2;
    int yaw;
    short* p = (short*)0x1F800350;
    vs_battle_syncCameraAnglesFromPosition(&camera);
    yaw = camera.values.yaw;
    temp_s2 = rcos(yaw);
    temp_a0 = rsin(yaw);
    p[0] = temp_s2;
    p[2] = temp_a0;
    p[8] = -temp_a0;
    p[10] = temp_s2;
    D_800EB9B8->unk22 = yaw;
    return (void*)0x1F800350;
}

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C28AC);

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C2B0C);

INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C2E24);

#if defined(PERMUTER) || defined(OBJDIFF)
void func_800C4650(char* arg0, int arg1)
{
    typedef void (*func_8009FD5C_full_t)(int, int, int);
    typedef void (*func_8009FE74_full_t)(int, int);
    int i;
    int value;
    char* current;
    D_800EB9B8_t** statep;

    statep = &D_800EB9B8;
    current = arg0 + 0xA;
    for (i = 0; i < arg1; ++i) {
        value = (u_char)current[-1];
        if ((value >> 4) == 0) {
            if (func_800C1564((char*)*statep + 0x10, arg0) != 0) {
                ((func_8009FD5C_full_t)func_8009FD5C)(value, 0, (signed char)current[0]);
            } else {
                ((func_8009FE74_full_t)func_8009FE74)(value, (signed char)current[0]);
            }
        }
        current += 0x18;
        arg0 += 0x18;
    }
}
#else
INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", func_800C4650);
#endif

int vs_battle_mapStickDeadZone(int arg0)
{
    if (arg0 < 64) {
        return arg0 - 64;
    }
    if (arg0 >= 192) {
        return arg0 - 192;
    }
    return 0;
}

int func_800C4734(void)
{
    if ((D_800EB9B8 == NULL) || (D_800EB9B8->unk2A != 0)) {
        return 0;
    }
    if ((D_800EB9B8->unk3E == 0) || (vs_main_buttonsState & 0x80)) {
        return 1;
    }
    return 2;
}

INCLUDE_RODATA("build/src/BATTLE/BATTLE.PRG/nonmatchings/58578", D_80069860);
