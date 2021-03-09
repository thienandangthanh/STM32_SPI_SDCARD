#ifndef __FAT_UTILS_H__
#define __FAT_UTILS_H__
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
/* Application functions */
// read file with path
void SD_read(const TCHAR *path);
// write content to path
void SD_write(const TCHAR *path, const TCHAR *content);
// list files, folders under path
void SD_list(const TCHAR *path);
// get stats (total space, free space) of SD card
void SD_stats(void);

#endif // __FAT_UTILS_H__
