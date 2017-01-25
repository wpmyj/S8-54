#include "defines.h" 
#include "globals.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <usbh_def.h>
#include <ff_gen_drv.h>
#include <usbh_diskio.h>
#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "FlashDrive.h"
#include "main.h"

#include <ctype.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static FATFS USBDISKFatFs;
static char USBDISKPath[4];
static bool isProcessing = false;   // true, ����� ���������� ����������� �����

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id)
{
    switch(id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_CLASS_ACTIVE:
            isProcessing = true;
            state = State_Mount;
            if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 1) != FR_OK)
            {
                state = State_WrongFlash;
            }
            break;

        case HOST_USER_CLASS_SELECTED:
            break;

        case HOST_USER_CONNECTION:
            state = State_Mount;
            f_mount(NULL, (TCHAR const*)"", 0);
            break;

        case HOST_USER_DISCONNECTION:
            break;

        default:
            break;
    }

#ifdef _MS_VS
    phost = phost;
#endif
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_Mount(void)
{

    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive_Init(void)
{
    if(FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == FR_OK) 
    {
        USBH_StatusTypeDef res = USBH_Init(&handleUSBH, USBH_UserProcess, 0);
        res = USBH_RegisterClass(&handleUSBH, USBH_MSC_CLASS);
        res = USBH_Start(&handleUSBH);
    }
    else
    {
        // ���� ��������, ���� usbh driver �� ������� ����������������
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_Update(void)
{
    USBH_Process(&handleUSBH);
    return isProcessing;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_AppendStringToFile(const char* string)
{
#ifdef _MS_VS
    string = 0;
#endif

    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void WriteToFile(FIL *file, char *string)
{
    //    uint bytesWritten;
    f_open(file, "list.txt", FA_OPEN_EXISTING);
    //f_write(file, string, strlen(string), (void*)&bytesWritten);
    f_puts(string, file);
    f_close(file);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive_GetNumDirsAndFiles(const char* fullPath, int *numDirs, int *numFiles)
{
    FILINFO fno;
    DIR dir;

    *numDirs = 0;
    *numFiles = 0;
    

    char nameDir[_MAX_LFN + 1];
    memcpy(nameDir, fullPath, strlen(fullPath));
    nameDir[strlen(fullPath)] = '\0';

    char lfn[(_MAX_LFN + 1)];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);

    if (f_opendir(&dir, nameDir) == FR_OK)
    {
        int numReadingElements = 0;
        bool alreadyNull = false;
        bool run = true;
        while (run)
        {
            if (f_readdir(&dir, &fno) != FR_OK)
            {
                break;
            }
            if (fno.fname[0] == 0)
            {
                if(alreadyNull)
                {
                    break;
                }
                alreadyNull = true;
                continue;
            }
            numReadingElements++;
            char *fn = *fno.lfname ? fno.lfname : fno.fname;
            if (fn[0] != '.')
            {
                if (fno.fattrib & AM_DIR)
                {
                    (*numDirs)++;
                }
                else
                {
                    (*numFiles)++;
                }
            }
        }
        f_closedir(&dir);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_GetNameDir(const char *fullPath, int numDir, char *nameDirOut, StructForReadDir *s)
{
#ifdef _MS_VS
    numDir = numDir;
#endif

    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

    s->fno.lfname = s->lfn;
    s->fno.lfsize = sizeof(s->lfn);

    DIR *pDir = &s->dir;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        int numDirs = 0;
        FILINFO *pFNO = &s->fno;
        bool alreadyNull = false;
        bool run = true;
        while (run)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameDirOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameDirOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }
#ifndef _MS_VS
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (numDir == numDirs && (pFNO->fattrib & AM_DIR))
            {
                strcpy(nameDirOut, fn);
                return true;
            }
#endif
            if ((pFNO->fattrib & AM_DIR) && (pFNO->fname[0] != '.'))
            {
                numDirs++;
            }
        }
    }
    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_GetNextNameDir(char *nameDirOut, StructForReadDir *s)
{
    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    bool alreadyNull = false;
    bool run = true;
    while (run)
    {
        if (f_readdir(pDir, pFNO) != FR_OK)
        {
            *nameDirOut = '\0';
            f_closedir(pDir);
            return false;
        }
        else if (pFNO->fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameDirOut = '\0';
                f_closedir(pDir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
#ifndef _MS_VS
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (pFNO->fattrib & AM_DIR)
            {
                strcpy(nameDirOut, fn);
                return true;
            }
#endif
        }
    }

    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive_CloseCurrentDir(StructForReadDir *s)
{
    f_closedir(&s->dir);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *s)
{
#ifdef _MS_VS
    numFile = numFile;
#else
    int numFiles = 0;
#endif

    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

    s->fno.lfname = s->lfn;
    s->fno.lfsize = sizeof(s->lfn);

    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        bool alreadyNull = false;
        bool run = true;
        while (run)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameFileOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameFileOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }
#ifndef _MS_VS
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (numFile == numFiles && (pFNO->fattrib & AM_DIR) == 0)
            {
                strcpy(nameFileOut, fn);
                return true;
            }
            if ((pFNO->fattrib & AM_DIR) == 0 && (pFNO->fname[0] != '.'))
            {
                numFiles++;
            }
#endif
        }
    }
    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_GetNextNameFile(char *nameFileOut, StructForReadDir *s)
{
    bool alreadyNull = false;
    bool run = true;
    while (run)
    {
        if (f_readdir(&s->dir, &s->fno) != FR_OK)
        {
            *nameFileOut = '\0';
            f_closedir(&s->dir);
            return false;
        }
        if (s->fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameFileOut = '\0';
                f_closedir(&s->dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
            FILINFO *pFNO = &s->fno;
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if ((pFNO->fattrib & AM_DIR) == 0 && pFNO->fname[0] != '.')
            {
#ifndef _MS_VS
                strcpy(nameFileOut, fn);
#endif
                return true;
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_OpenNewFileForWrite(const char* fullPathToFile, StructForWrite *structForWrite)
{
    if (f_open(&structForWrite->fileObj, fullPathToFile, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        return false;
    }
    structForWrite->sizeData = 0;
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_WriteToFile(uint8* data, int sizeData, StructForWrite *structForWrite)
{
    while (sizeData > 0)
    {
        int dataToCopy = sizeData;
        if (sizeData + structForWrite->sizeData > SIZE_FLASH_TEMP_BUFFER)
        {
            dataToCopy = SIZE_FLASH_TEMP_BUFFER - structForWrite->sizeData;
        }
        sizeData -= dataToCopy;
        memcpy(structForWrite->tempBuffer + structForWrite->sizeData, data, dataToCopy);
        data += dataToCopy;
        structForWrite->sizeData += dataToCopy;
        if (structForWrite->sizeData == SIZE_FLASH_TEMP_BUFFER)
        {
            uint wr = 0;
            if (f_write(&structForWrite->fileObj, structForWrite->tempBuffer, structForWrite->sizeData, &wr) != FR_OK || structForWrite->sizeData != (int)wr)
            {
                return false;
            }
            structForWrite->sizeData = 0;
        }
    }

    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_CloseFile(StructForWrite *structForWrite)
{
    if (structForWrite->sizeData != 0)
    {
        uint wr = 0;
        if (f_write(&structForWrite->fileObj, structForWrite->tempBuffer, structForWrite->sizeData, &wr) != FR_OK || structForWrite->sizeData != (int)wr)
        {
            f_close(&structForWrite->fileObj);
            return false;
        }
    }
    f_close(&structForWrite->fileObj);
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void ToLower(char *str)
{
    while (*str)
    {
        *str = tolower(*str);
        str++;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_FileExist(char *fileName)
{
    char nameFile[255];
    char file[255];
    strcpy(file, fileName);
    ToLower(file);
    StructForReadDir strd;

    if (FDrive_GetNameFile("\\", 0, nameFile, &strd))
    {
        ToLower(nameFile);
        if (strcmp(file, nameFile) == 0)
        {
            return true;
        }
        while (FDrive_GetNextNameFile(nameFile, &strd))
        {
            ToLower(nameFile);  
            if (strcmp(file, nameFile) == 0)
            {
                return true;
            }
        }
    }

    return false;
}
