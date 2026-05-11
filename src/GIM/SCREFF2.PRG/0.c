#include "common.h"
#include "0.h"
#include "../../SLUS_010.40/31724.h"
#include <libgte.h>

void func_8007DFF0(int, int, int);
void func_8007E0A8(int, int, int);
u_long* func_800C0230(int, int, int, u_long*);

extern int* D_1F800000[];
extern int vs_main_frameBuf;

void func_800F9800(int arg0)
{
    D_800EB9B4->unk2 = arg0;
    if (D_800EB9B4->unk1 == 0) {
        if (arg0 != 0) {
            D_800EB9B4->unk1 = 1U;
            func_8007DFF0(0x1B, 5, 6);
        }
    } else if (arg0 == 0) {
        D_800EB9B4->unk1 = 0U;
        func_8007E0A8(0x1B, 5, 6);
    }
}

// Milestone 3 first target: display-list setup; generated asm is non-handwritten.
#if defined(PERMUTER) || defined(OBJDIFF)
void func_800F986C(void)
{
    u_long* prim;
    u_long* ot;
    u_long** scratch;
    int outer;
    int y;
    int x;
    int part;
    int tex;
    int xy;
    int fbX;
    int offsetA;
    int offsetB;

    scratch = (u_long**)0x1F800000;
    ot = scratch[1];

    offsetA = 0xDE0000;
    offsetB = 0xC20000;
    for (outer = 0xC0; outer >= 0; outer -= 0x20) {
        for (y = 0x100; y >= 0; y -= 0x40) {
            tex = ((((y + 0x2C0) & 0x3FF) >> 6) | 0x170);
            for (x = 0x40; x >= 0; x -= 0x20) {
                if (x == 0x40) {
                    prim = func_800C0230(0x160, (y + 2) | offsetA, 0x40040, ot);
                    xy = (outer + 0x1C) << 8;
                } else {
                    prim = func_800C0230(0x160, (y + x + 0x1E) | offsetB, 0x1C0004, ot);
                    xy = (x + 0x1C) | (outer << 8);
                }
                prim[1] = 0xE1000200 | tex;
                prim[4] = xy;
            }
        }
        offsetA += 0xFFE00000;
        offsetB += 0xFFE00000;
    }

    offsetB = 0xC20000;
    for (outer = 0xC0; outer >= 0; outer -= 0x20) {
        for (y = 0x120; y >= 0; y -= 0x20) {
            for (part = 0; part < 3; ++part) {
                if (part == 0) {
                    prim = func_800C0230(0x160, (y + 2) | offsetB, 0x1C001C, ot);
                } else if (part == 1) {
                    prim =
                        func_800C0230(0x160, (y - 2) | ((outer - 2) << 16), 0x200020, ot);
                } else {
                    prim = func_800C0230(0x60, y | (outer << 16), 0x200020, ot);
                }

                tex = ((((y & 0x1C0) + 0x2C0) & 0x3FF) >> 6);
                if (part == 0) {
                    tex |= 0x170;
                } else {
                    tex |= 0x110;
                }
                prim[1] = 0xE1000200 | tex;
                prim[4] = (y & 0x20) | (outer << 8);
            }
        }
        offsetB += 0xFFE00000;
    }

    prim = scratch[0];

    x = D_800EB9B4->unk2 - 3;
    prim[0] = (ot[x] & 0xFFFFFF) | 0x07000000;
    prim[1] = 0xE30402C0;
    prim[2] = 0xE4077FFF;
    prim[3] = 0xE50802C0;
    prim[4] = 0x60000000;
    prim[5] = 0;
    prim[6] = 0xE00140;
    prim[7] = 0xE6000001;
    ot[x] = ((u_long)prim << 8) >> 8;
    prim += 8;

    fbX = 0;
    if (vs_main_frameBuf == 0) {
        fbX = 0x140;
    }

    prim[0] = (ot[0] & 0xFFFFFF) | 0x04000000;
    prim[1] = 0xE3000000 | fbX;
    prim[2] = 0xE4037C00 | (fbX + 0x13F);
    prim[3] = 0xE5000000 | fbX;
    prim[4] = 0xE6000000;
    ot[0] = ((u_long)prim << 8) >> 8;

    ((u_long**)0x1F800000)[0] = prim + 5;
}
#else
INCLUDE_ASM("build/src/GIM/SCREFF2.PRG/nonmatchings/0", func_800F986C);
#endif

void func_800F9BC0(short arg0, short arg1)
{
    D_800EB9B4->unk4 = arg0;
    D_800EB9B4->unk6 = arg1;
}

// Generated asm marks this as handwritten and uses raw GTE instructions.
INCLUDE_ASM("build/src/GIM/SCREFF2.PRG/nonmatchings/0", func_800F9BD8);

void func_800F9DE8(int arg0) { D_800EB9B4->unk8 = arg0; }

int func_800F9DF8(int arg0, int arg1)
{
    int var_s1;

    var_s1 = 8 - ((arg0 + 8) & 0xF);
    if (var_s1 < 0) {
        var_s1 = -var_s1;
    }
    var_s1 += 0x40;
    return ((((((rcos(arg0 << 6) * arg1 * 0xB) >> 0xF) * var_s1) >> 6) + 0xA0) & 0xFFFF)
         | ((((((rsin(arg0 << 6) * arg1) >> 0xC) * var_s1) >> 6) + 0x70) << 0x10);
}

int* func_800F9EBC(int arg0, int arg1, int* arg2, int* arg3)
{
    int var_s2;

    for (var_s2 = arg1; var_s2 < arg1 + 4; ++var_s2) {
        arg2[0] = (*arg3 & 0xFFFFFF) | 0x06000000;
        arg2[1] = 0x32FFFFFF;
        arg2[2] = arg0;
        arg2[3] = 0xA0A0A0;
        arg2[4] = func_800F9DF8(var_s2 + 1, 0x80);
        arg2[5] = 0xA0A0A0;
        arg2[6] = func_800F9DF8(var_s2, 0x80);
        *arg3 = (u_int)((long)arg2 << 8) >> 8;
        arg2 += 7;
    }
    return arg2;
}

void func_800F9FB8(void)
{
    int* var_s2;
    int i;
    int* temp_s4;

    temp_s4 = D_1F800000[2];
    var_s2 = func_800F9EBC(0xE00140, 6, D_1F800000[0], temp_s4);
    var_s2 = func_800F9EBC(0xE00000, 0x16, var_s2, temp_s4);
    var_s2 = func_800F9EBC(0, 0x26, var_s2, temp_s4);
    var_s2 = func_800F9EBC(0x140, 0x36, var_s2, temp_s4);

    for (i = 0; i < 0x40; ++i) {
        var_s2[0] = (*temp_s4 & 0xFFFFFF) | 0x09000000;
        var_s2[1] = 0xE1000220;
        var_s2[2] = 0x3AA0A0A0;
        var_s2[3] = func_800F9DF8(i + 1, 0x80);
        var_s2[4] = 0xA0A0A0;
        var_s2[5] = func_800F9DF8(i, 0x80);
        var_s2[6] = 0;
        var_s2[7] = func_800F9DF8(i + 1, 0x60);
        var_s2[8] = 0;
        var_s2[9] = func_800F9DF8(i, 0x60);
        *temp_s4 = (u_int)((long)var_s2 << 8) >> 8;
        var_s2 += 10;
    }
    D_1F800000[0] = var_s2;
}
