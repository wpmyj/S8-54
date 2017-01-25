#pragma once
#include "defines.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <ff.h>

#ifdef _MS_VS
#undef _WIN32
#endif

#define SIZE_FLASH_TEMP_BUFFER 512
typedef struct
{
    uint8 tempBuffer[SIZE_FLASH_TEMP_BUFFER];
    int sizeData;
    FIL fileObj;
} StructForWrite;

typedef struct
{
    char nameDir[_MAX_LFN + 1];
    char lfn[(_MAX_LFN + 1)];
    FILINFO fno;
    DIR dir;
} StructForReadDir;


bool    FDrive_Init(void);
bool    FDrive_Update(void);
void    FDrive_GetNumDirsAndFiles(const char* fullPath, int *numDirs, int *numFiles);
bool    FDrive_GetNameDir(const char* fuulPath, int numDir, char *nameDirOut, StructForReadDir *sfrd);
bool    FDrive_GetNextNameDir(char *nameDirOut, StructForReadDir *sfrd);
void    FDrive_CloseCurrentDir(StructForReadDir *sfrd);
bool    FDrive_GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *sfrd);
bool    FDrive_GetNextNameFile(char *nameFileOut, StructForReadDir *sfrd);
bool    FDrive_OpenNewFileForWrite(const char* fullPathToFile, StructForWrite *structForWrite); // Функция создаст файл для записи. Если такой файл уже существует, сотрёт его, заменит новым нулевой длины и откроет его
bool    FDrive_WriteToFile(uint8* data, int sizeData, StructForWrite *structForWrite);
bool    FDrive_CloseFile(StructForWrite *structForWrite);
bool    FDrive_AppendStringToFile(const char* string);
bool    FDrive_IsConnected(void);
bool    FDrive_FileExist(char *fileName);


int     FDrive_OpenFileForRead(char *fileName);
int     FDrive_ReadFromFile(int numBytes, uint8 *buffer);   // Считывает из открытого файла numBytes байт. Возвращает число реально считанных байт
void    FDrive_CloseOpenedFile(void);
