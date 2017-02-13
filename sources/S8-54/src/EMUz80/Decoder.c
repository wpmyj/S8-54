#include "Decoder.h"

#define BOOL int
#define APIENTRY
#define HMODULE int
#define DWORD int
#define LPVOID int

#include "CPU/instr.h"
#include "CPU/registers.h"
#include "commonZ80.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutStruct *out = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
#ifdef _MSC_VER
    hModule = 0;
    lpReserved = 0;

    switch(ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
#else
		return 1;
#endif
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) void __cdecl InitEMU(uint8 *RAM)
{
    /*
    out->regs = &regs;
    out->regsAlt = &regsAlt;
    */
    RAMZ80 = RAM;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __cdecl Decode(int address, OutStruct *out_)
{
    out = out_;
    out->numAddresses = 0;
    out->numOpCodes = 0;
    TRANSCRIPT[0] = 0;
    out->flags[0] = 0;
    out->tackts = 0;
    out->comment[0] = 0;
    out->address = address;
    out->regs = &regs;
    out->regsAlt = &regsAlt;

    PC = (uint16)address;

    // If RunCommand() return >= 0, decoding fail
    return DecodeCommand();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int Run(int address)
{
    PC = (uint16)address;

    return RunCommand();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int RunNext(void)
{
    return RunCommand();
}
