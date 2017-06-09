#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspSound;


// СЕРВИС - ЗВУК /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Governor mgServiceSoundVolume =
{
    Item_Governor, &mspSound, 0,
    {
        "Громкость", "Volume",
        "Установка громкости звука",
        "Set the volume"
    },
    &SOUND_VOLUME, 0, 100, EmptyFuncVV
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Choice mcServiceSoundEnable =
{
    Item_Choice, &mspSound, 0,
    {
        "Звук", "Sound",
        "Включение/выключение звука",
        "Inclusion/switching off of a sound"
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&SOUND_ENABLED
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Page mspSound =
{
    Item_Page, &pService, 0,
    {
        "ЗВУК", "SOUND",
        "В этом меню можно настроить громкость звука",
        "In this menu, you can adjust the volume"
    },
    Page_ServiceSound,
    {
        (void*)&mcServiceSoundEnable,
        (void*)&mgServiceSoundVolume
    }

};
