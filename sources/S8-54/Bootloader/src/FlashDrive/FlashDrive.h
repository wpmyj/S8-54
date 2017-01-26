#pragma once

#include "defines.h"

#include <ff.h>

typedef enum
{
    StateDisk_Idle,
    StateDisk_Start
} StateDisk;

typedef struct
{
    FATFS USBDISKFatFS;
    char USBDISKPath[4];
    StateDisk stateDisk;
    FIL file;
    int connection;
    int active;
} FDriveStruct;

void    FDrive_Init(FDriveStruct *fds);
bool    FDrive_Update(void);
bool    FDrive_FileExist(char *fileName);
int     FDrive_OpenFileForRead(char *fileName);
int     FDrive_ReadFromFile(int numBytes, uint8 *buffer);   // Считывает из открытого файла numBytes байт. Возвращает число реально считанных байт
void    FDrive_CloseOpenedFile(void);


