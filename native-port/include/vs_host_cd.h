#ifndef VS_HOST_CD_H
#define VS_HOST_CD_H

#include <stddef.h>
#include <stdint.h>

typedef struct VsHostCd VsHostCd;

typedef enum VsHostCdStatus {
    VS_HOST_CD_IDLE = 0,
    VS_HOST_CD_READING = 1,
    VS_HOST_CD_STREAMING_XA = 2,
    VS_HOST_CD_ERROR = -1,
} VsHostCdStatus;

int vs_host_cd_set_asset_root(VsHostCd* cd, const char* asset_root);
int vs_host_cd_read_sectors(VsHostCd* cd, uint32_t lba, uint32_t sector_count, void* output, size_t output_size);
int vs_host_cd_seek(VsHostCd* cd, uint32_t lba);
int vs_host_cd_start_xa(VsHostCd* cd, uint32_t lba, uint32_t sector_count);
void vs_host_cd_stop_xa(VsHostCd* cd);
VsHostCdStatus vs_host_cd_status(const VsHostCd* cd);

#endif
