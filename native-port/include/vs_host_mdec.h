#ifndef VS_HOST_MDEC_H
#define VS_HOST_MDEC_H

#include <stddef.h>
#include <stdint.h>

typedef struct VsHostMdec VsHostMdec;

typedef struct VsHostMdecEnv {
    uint8_t iq_y[64];
    uint8_t iq_c[64];
    int16_t dct[64];
} VsHostMdecEnv;

typedef void (*VsHostMdecCallback)(void* user_data);

void vs_host_mdec_reset(VsHostMdec* mdec, int mode);
void vs_host_mdec_put_env(VsHostMdec* mdec, const VsHostMdecEnv* env);
void vs_host_mdec_build_vlc_table(VsHostMdec* mdec, uint16_t* table, size_t table_entries);
int vs_host_mdec_decode_vlc(VsHostMdec* mdec, const uint32_t* bitstream, uint32_t* words, uint16_t* table);
void vs_host_mdec_submit_input(VsHostMdec* mdec, uint32_t* words, int mode);
void vs_host_mdec_read_output(VsHostMdec* mdec, uint32_t* words, size_t word_count);
void vs_host_mdec_set_input_callback(VsHostMdec* mdec, VsHostMdecCallback callback, void* user_data);
void vs_host_mdec_set_output_callback(VsHostMdec* mdec, VsHostMdecCallback callback, void* user_data);

#endif
