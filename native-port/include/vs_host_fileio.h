#ifndef VS_HOST_FILEIO_H
#define VS_HOST_FILEIO_H

#include <stddef.h>
#include <stdint.h>

typedef struct VsHostFileIo VsHostFileIo;

typedef struct VsHostFileInfo {
    char name[32];
    uint32_t size;
} VsHostFileInfo;

int vs_host_file_open(VsHostFileIo* fileio, const char* path, int mode);
int vs_host_file_close(VsHostFileIo* fileio, int handle);
int vs_host_file_read(VsHostFileIo* fileio, int handle, void* output, size_t size);
int vs_host_file_write(VsHostFileIo* fileio, int handle, const void* input, size_t size);
int vs_host_file_lseek(VsHostFileIo* fileio, int handle, int offset, int whence);
int vs_host_file_first(VsHostFileIo* fileio, const char* pattern, VsHostFileInfo* info);
int vs_host_file_next(VsHostFileIo* fileio, VsHostFileInfo* info);
int vs_host_file_erase(VsHostFileIo* fileio, const char* path);

#endif
