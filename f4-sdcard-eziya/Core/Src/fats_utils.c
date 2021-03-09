#include "fatfs_utils.h"

// FRESULT description
static void fres_des(FRESULT fres) {
	switch (fres) {
	case FR_OK:
		printf("%s\t%s\n", "FR_OK", "Succeeded");
		break;
	case FR_DISK_ERR:
		printf("%s\t%s\n", "FR_DISK_ERR",
				"A hard error occurred in the low level disk I/O layer");
		break;
	case FR_INT_ERR:
		printf("%s\t%s\n", "FR_INT_ERR", "Assertion failed");
		break;
	case FR_NOT_READY:
		printf("%s\t%s\n", "FR_NOT_READY", "The physical drive cannot work");
		break;
	case FR_NO_FILE:
		printf("%s\t%s\n", "FR_NO_FILE", "Could not find the file");
		break;
	case FR_NO_PATH:
		printf("%s\t%s\n", "FR_NO_PATH", "Could not find the path");
		break;
	case FR_INVALID_NAME:
		printf("%s\t%s\n", "FR_INVALID_NAME",
				"The path name format is invalid");
		break;
	case FR_DENIED:
		printf("%s\t%s\n", "FR_DENIED",
				"Access denied due to prohibited access or directory full");
		break;
	case FR_EXIST:
		printf("%s\t%s\n", "FR_EXIST",
				"Access denied due to prohibited access");
		break;
	case FR_INVALID_OBJECT:
		printf("%s\t%s\n", "FR_INVALID_OBJECT",
				"The file/directory object is invalid");

		break;
	case FR_WRITE_PROTECTED:
		printf("%s\t%s\n", "FR_WRITE_PROTECTED",
				"The physical drive is write protected");

		break;
	case FR_INVALID_DRIVE:
		printf("%s\t%s\n", "FR_INVALID_DRIVE",
				"The logical drive number is invalid");
		break;
	case FR_NOT_ENABLED:
		printf("%s\t%s\n", "FR_NOT_ENABLED", "The volume has no work area");
		break;
	case FR_NO_FILESYSTEM:
		printf("%s\t%s\n", "FR_NO_FILESYSTEM", "There is no valid FAT volume");
		break;
	case FR_MKFS_ABORTED:
		printf("%s\t%s\n", "FR_MKFS_ABORTED",
				"The f_mkfs() aborted due to any problem");
		break;
	case FR_TIMEOUT:
		printf("%s\t%s\n", "FR_TIMEOUT",
				"Could not get a grant to access the volume within defined period");
		break;
	case FR_LOCKED:
		printf("%s\t%s\n", "FR_LOCKED",
				"The operation is rejected according to the file sharing policy");
		break;
	case FR_NOT_ENOUGH_CORE:
		printf("%s\t%s\n", "FR_NOT_ENOUGH_CORE",
				"LFN working buffer could not be allocated");
		break;
	case FR_TOO_MANY_OPEN_FILES:
		printf("%s\t%s\n", "FR_TOO_MANY_OPEN_FILES",
				"Number of open files > _FS_LOCK");
		break;
	case FR_INVALID_PARAMETER:
		printf("%s\t%s\n", "FR_INVALID_PARAMETER",
				"Given parameter is invalid");
		break;
	default:
		printf("Unknown error!\n");
	}
}

/* Application functions */

void SD_read(const TCHAR *path) {
	FIL fil;
	FRESULT fres;

	fres = f_open(&fil, path, FA_READ);
	if (fres != FR_OK) {
		fres_des(fres);
		return;
	}
	DWORD file_size = (DWORD) f_size(&fil);
	DWORD chunk = 0;
	DWORD index = 0;
	UINT readBytes = 0;
	TCHAR *readBuffer;

//	printf("Reading file \"%s\"\n", path);
//	printf("Size of \"%s\": %luB\n", path, file_size);

	do {
		chunk = (file_size >= 512) ? 512 : file_size;
		file_size -= chunk;
		readBuffer = (TCHAR*) malloc(chunk * sizeof(TCHAR));

		fres = f_lseek(&fil, index);
		if (fres != FR_OK) {
			fres_des(fres);
			f_close(&fil);
			return;
		}

		fres = f_read(&fil, readBuffer, chunk, &readBytes);
		if (fres != FR_OK) {
			fres_des(fres);
			free(readBuffer);
			break;
		} else {
			if (readBytes == 0) {
				printf("End Of File!\n");
				free(readBuffer);
				break;
			} else {
				printf("%s", readBuffer);
				// for printing in hex
				/*
				 char *cp = readBuffer;
				 for (; *cp != '\0'; ++cp)
				 	 printf("%02X ", *cp);
				 */
				index += chunk;
			}
		}
		free(readBuffer);
	} while (file_size > 0);
	printf("\n");

	f_close(&fil);
}

void SD_write(const TCHAR *path, const TCHAR *content) {
	FRESULT fres;
	FIL fil;

	fres = f_open(&fil, path,
	FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	if (fres != FR_OK) {
		fres_des(fres);
		return;
	} else {
		printf("Open file \"%s\" successfully!\n", path);
	}
	UINT writtenBytes;
	fres = f_write(&fil, content, strlen(content), &writtenBytes);
	if (fres == FR_OK) {
		printf("Wrote %i bytes to \"%s\"!\n", writtenBytes, path);
	} else {
		fres_des(fres);
	}
	f_close(&fil);

}

void SD_list(const TCHAR *path) {
	DIR dir;
	FILINFO fileInfo;
	FRESULT fres;
	uint8_t i = 0;

	fres = f_opendir(&dir, path);
	if (fres != FR_OK) {
		fres_des(fres);
		return;
	}
	if (fres == FR_OK) {
		printf("Content under \"%s\":\n", path);
		while (1) {
			fres = f_readdir(&dir, &fileInfo);
			if (fres == FR_OK && fileInfo.fname[0] != '\0') {
				i += 1;
				printf("#%d\t%s", i, fileInfo.fname);
				if (fileInfo.fattrib & AM_DIR)
					printf("\t[DIR]");
				printf("\n");
			} else {
				if (i == 0) {
					printf("[empty]\n");
				}
				break;
			}
		}
	}
	fres = f_closedir(&dir);
	if (fres != FR_OK)
		fres_des(fres);
}

void SD_stats(void) {
	DWORD free_clusters, free_sectors, total_sectors;
	FATFS *getFreeFs;
	FRESULT fres;

	fres = f_getfree("", &free_clusters, &getFreeFs);
	if (fres != FR_OK) {
		fres_des(fres);
		return;
	}
	total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
	free_sectors = free_clusters * getFreeFs->csize;
	printf("SD card stats:\n");
	printf("%10lu KiB total drive space.\n", total_sectors / 2);
	printf("%10lu KiB free space.\n", free_sectors / 2);
}
