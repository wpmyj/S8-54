#include "mainZ80.h"
#include "definesZ80.h"
#include "TimerZ80.h"
#include "CPU/instr.h"
#include "Decoder.h"
#include "DataBase.h"
#include "RAM48.h"

#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "VCP/VCP.h"
#include "Ethernet/Ethernet.h"
#include "FlashDrive/FlashDrive.h"
#include "Utils/GlobalFunctions.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawParams(int x, int y, OutStruct *params);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main1(void)
{
    /*
    Сначала настроим таймер тиков таким образом, чтобы он отсчитывал нам тики.
    */

    TimerZ80_Init();

    TimerZ80_StartTicks();

//    while(true)
//    {
        uint tackts = RunCommand();
        TimerZ80_WaitForNTacts(tackts);
//    }
		return 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void Disable_IfNessessary(void)
{
    if (gBF.disablePower == 1)
    {
        Settings_Save();
        Log_DisconnectLoggerUSB();
        Panel_TransmitData(0x04);
        bool run = true;
        while (run)
        {
        };
    }
}


#define WIDTH 256
#define HEIGHT 192

#define X0 (320 / 2 - WIDTH / 2)
#define Y0 (240 / 2 - HEIGHT / 2)



//------------------------------------------------------------------------------------------------------------------------------------------------------]
void Z80_Run(void)
{
    OutStruct params;

    RAM48_Init();

    InitEMU(RAM48);

    Hardware_Init();
    VCP_Init();
    Settings_Load(false);
    Timer_PauseOnTime(250);
    Display_Init();
    Ethernet_Init();
    Menu_Init();
    
    Section sections[NUM_SECTIONS];

    DataBase_Init(sections);

    /*
    for (int i = 0; i < 1; i++)
    {
        bool rez = Decode(0, &params) != 0;
    }
    */

    uint16 addresses[21] = {
        0, 1, 2, 5, 8, 0x0b, 0x0e, 0x10, 0x18, 0x1b, 0x1c, 0x1f, 0x20, 0x23, 0x28, 0x30,
        0x31, 0x34, 0x35, 0x38, 0x39
    };

    bool run = true;
    while (run)
    {
        Timer_StartMultiMeasurement();
        Ethernet_Update(0);
        FDrive_Update();
        Painter_BeginScene((Color)(rand() % 16));
        Painter_FillRegionC(X0, Y0, WIDTH, HEIGHT, gColorBack);

        Painter_SetColor(gColorFill);

        for (int i = 0; i < 21; i++)
        {
            Decode(addresses[i], &params);
            DrawParams(X0 + 1, Y0 + i * 9, &params);
        }

        Painter_EndScene();
        Panel_Update();
        Menu_UpdateInput();
        Disable_IfNessessary();
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawParams(int x, int y, OutStruct *params)
{
    char buffer[10];
    Painter_DrawText(x, y, Hex16toString((uint16)params->address, buffer, true));

    for (int i = 0; i < params->numOpCodes; i++)
    {
        Painter_DrawText(x + 27 + i * 10, y, Hex8toString(params->opCodes[i], buffer, true));
    }

    Painter_DrawText(x + 75, y, params->mnemonic);
}
