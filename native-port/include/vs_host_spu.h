#ifndef VS_HOST_SPU_H
#define VS_HOST_SPU_H

#include <stddef.h>
#include <stdint.h>

typedef struct VsHostSpu VsHostSpu;

typedef struct VsHostSpuVoiceCommand {
    uint16_t voice;
    uint16_t register_id;
    uint16_t value;
} VsHostSpuVoiceCommand;

void vs_host_spu_reset(VsHostSpu* spu);
void vs_host_spu_transfer_to_ram(VsHostSpu* spu, uint32_t spu_address, const void* data, size_t size);
void vs_host_spu_transfer_from_ram(VsHostSpu* spu, uint32_t spu_address, void* data, size_t size);
void vs_host_spu_apply_voice_command(VsHostSpu* spu, const VsHostSpuVoiceCommand* command);
void vs_host_spu_tick_frame(VsHostSpu* spu);

#endif
