#include "defines.h"
#include "Log.h"
#include "Menu/FileManager.h"
#include "Display/Display.h"

#ifdef _MS_VS
#undef _WIN32
#endif

#include <usbh_def.h>
#include <ff_gen_drv.h>
#include <usbh_diskio.h>
#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Hardware/Timer2.h"
#include "FlashDrive.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static FATFS USBDISKFatFs;
static char USBDISKPath[4];
bool gFlashDriveIsConnected = false;


extern void ChangeStateFlashDrive(void);
static void FuncDrawDisplay(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id)
{
    switch(id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_CLASS_ACTIVE:
            gBF.needToMountFlash = 1;

            /*
            if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 1) != FR_OK)
            {
                Display_ShowWarning(WrongFileSystem);
            }
            else
            {
                gFlashDriveIsConnected = true;
                FM_Init();
                ChangeStateFlashDrive();
            }
            */
            break;

        case HOST_USER_CLASS_SELECTED:
            break;

        case HOST_USER_CONNECTION:
            f_mount(NULL, (TCHAR const*)"", 0);
            break;

        case HOST_USER_DISCONNECTION:
            gFlashDriveIsConnected = false;
            ChangeStateFlashDrive();
            break;

        default:
            break;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive_Mount(void)
{
    FM_Init();
    ChangeStateFlashDrive();
    if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
    {
        LOG_ERROR("Не могу примонтировать диск");
    }
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
        LOG_ERROR("Can not %s", __FUNCTION__);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive_Update(void)
{
    if (gBF.needToMountFlash == 1)      // Если обнаружено физическое подключение внешнего диска
    {
        uint timeStart = gTimerMS;
        gBF.needToMountFlash = 0;
        Display_SetDrawMode(DrawMode_Hand, FuncDrawDisplay);
        Timer2_SetAndEnable(kTimerMountFlash, Display_Update, 10);
        if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 1) != FR_OK)
        {
            Display_ShowWarning(WrongFileSystem);
        }
        else
        {
            gFlashDriveIsConnected = true;
            FM_Init();
            ChangeStateFlashDrive();
        }
        while (gTimerMS - timeStart < 3000)
        {
        };
        Timer2_Disable(kTimerMountFlash);
        Display_SetDrawMode(DrawMode_Auto, 0);
    }
    else
    {
        USBH_Process(&handleUSBH);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncDrawDisplay(void)
{
    static uint timeStart = 0;
    static bool first = true;
    if (first)
    {
        timeStart = gTimerMS;
        first = false;
    }

    uint time = ((gTimerMS - timeStart) / 400) % 4;

    int width = 200;
    int height = 150;
    int x = 160 - width / 2;
    int y = 120 - height / 2;

    Painter_FillRegionC(x, y, width, height, gColorBack);
    Painter_DrawRectangleC(x, y, width, height, gColorFill);
    Painter_DrawStringInCenterRect(x, y, width, height - 10, "Обнаружено запоминающее устройство");
    char buffer[100];
    buffer[0] = 0;
    strcat(buffer, "Идёт подключение");
    for (int i = 0; i < time; i++)
    {
        strcat(buffer, " .");
    }
    Painter_DrawStringInCenterRect(x, y + 10, width, height - 10, buffer);
    Painter_EndScene();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_AppendStringToFile(const char* string)
{
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
        while (true)
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
        while (true)
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
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (numDir == numDirs && (pFNO->fattrib & AM_DIR))
            {
                strcpy(nameDirOut, fn);
                return true;
            }
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
    while (true)
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
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (pFNO->fattrib & AM_DIR)
            {
                strcpy(nameDirOut, fn);
                return true;
            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FDrive_CloseCurrentDir(StructForReadDir *s)
{
    f_closedir(&s->dir);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *s)
{
    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

    s->fno.lfname = s->lfn;
    s->fno.lfsize = sizeof(s->lfn);

    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;
        while (true)
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
        }
    }
    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FDrive_GetNextNameFile(char *nameFileOut, StructForReadDir *s)
{
    FILINFO *pFNO = &s->fno;
    bool alreadyNull = false;
    while (true)
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
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if ((pFNO->fattrib & AM_DIR) == 0 && pFNO->fname[0] != '.')
            {
                strcpy(nameFileOut, fn);
                return true;
            }
        }
    }
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
