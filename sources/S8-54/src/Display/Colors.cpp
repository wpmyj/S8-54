#include "Colors.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Color gColorFill(Color::EMPTY);
Color gColorBack(Color::EMPTY);
Color gColorGrid(Color::EMPTY);
Color gColorChan[4] = {Color::EMPTY, Color::EMPTY, Color::EMPTY, Color::EMPTY};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Color Color::Cursors(Channel ch)
{
    return gColorChan[ch];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Color::Trig()
{
    if (TRIGSOURCE_EXT)
    {
        return gColorFill;
    }
    return gColorChan[(Channel)TRIGSOURCE];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Color::ChanAccum(Channel ch)
{
    return (ch == A) ? DATA_WHITE_ACCUM_A : DATA_WHITE_ACCUM_B;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Color::BorderMenu(bool shade)
{
    return MenuTitle(shade);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Color::MenuTitle(bool shade)
{
    return shade ?MENU_ITEM : MENU_TITLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Color::LightShadingText()
{
    return MenuTitle(false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Color::MenuItem(bool shade)
{
    return shade ? MENU_ITEM_DARK : MENU_ITEM;
}

Color Color::Contrast(Color color)
{
    uint16 colorValue = COLOR(color.value);
    if (R_FROM_COLOR(colorValue) > 16 || G_FROM_COLOR(colorValue) > 32 || B_FROM_COLOR(colorValue) > 16)
    {
        return BLACK;
    }
    return WHITE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool operator!=(const Color &left, const Color &right)
{
    return left.value != right.value;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool operator==(const Color &left, const Color &right)
{
    return left.value == right.value;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool operator>(const Color &left, const Color &right)
{
    return left.value > right.value;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ColorType::Init(bool forced)
{
    if (forced)
    {
        alreadyUsed = false;
    }

    if (!alreadyUsed)
    {
        alreadyUsed = true;                  // ѕризнак того, что начальные установки уже произведены

        uint16 colorValue = COLOR(color.value);

        red = (float)R_FROM_COLOR(colorValue);
        green = (float)G_FROM_COLOR(colorValue);
        blue = (float)B_FROM_COLOR(colorValue);

        SetBrightness();
        
        if (color.value == Color::DATA_A)
        {
            __IO int i = 0;
        }

        if (red == 0.0f && green == 0.0f && blue == 0.0f)
        {
            stepRed = 0.31f;
            stepGreen = 0.63f;
            stepBlue = 0.31f;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ColorType::SetBrightness(float brightness)
{
    if (brightness == -1.0f)
    {
        this->brightness = MaxFloat(red / 31.0f, green / 63.0f, blue / 31.0f);

        CalcSteps();
    }
    else
    {
        int delta = (int)((brightness + 0.0005f) * 100.0f) - (int)(brightness * 100.0f);

        if (delta > 0)
        {
            for (int i = 0; i < delta; i++)
            {
                BrightnessChange(1);
            }
        }
        else
        {
            for (int i = 0; i < -delta; i++)
            {
                BrightnessChange(-1);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
јлгоритм изменени€ €ркости.
1. »нициализаци€.
а. –ассчитать €ркость по принципу - €ркость равна относительной интенсивности самого €ркого цветового канала
б. –ассчитать шаг изменени€ цветовой составл€ющей каждого канала на 1% €ркости по формуле:
Ўаг = »нтенсивность канала * яркость, где яркость - относительна€ величина общей €ркости
в. ≈сли интенсивность всех каналов == 0, то дать полный шаг каждому каналу
2. ѕри изменнении €ркости на 1% мен€ть интенсивность каждого канала на Ўаг, расчитанный в предыдущем пункте.
3. ѕри изменени€ интенсивности цветового канала пересчитывать €ркость и шаг изменени€ каждого канала.
*/

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ColorType::BrightnessChange(int delta)
{
    if ((delta > 0 && brightness == 1.0f) || (delta < 0 && brightness == 0.0f))
    {
        return;
    }

    int sign = math.Sign(delta);

    brightness += sign * 0.01f;
    LIMITATION(brightness, 0.0f, 1.0f);

    red += sign * stepRed;
    green += sign * stepGreen;
    blue += sign * stepBlue;

    SetColor();

    if (stepRed < 0.01f && stepGreen < 0.01f && stepBlue < 0.01f)
    {
        stepRed = 0.31f;
        stepGreen = 0.63f;
        stepBlue = 0.31f;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ColorType::CalcSteps()
{
    stepRed = red / (brightness * 100.0f);
    stepGreen = green / (brightness * 100.0f);
    stepBlue = blue / (brightness * 100.0f);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ColorType::SetColor()
{
    COLOR(color.value) = MAKE_COLOR((int)red, (int)green, (int)blue);
    painter.SetPalette(color);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ColorType::ComponentChange(int delta)
{
    static const float maxs[4] = {0.0f, 31.0f, 63.0f, 31.0f};
    float * const pointers[4] = {0, &blue, &green, &red};
    int8 index = currentField;

    if (index >= 1 && index <= 3)
    {
        AddLimitationFloat(pointers[index], (float)math.Sign(delta), 0.0f, maxs[index]);
    }

    SetColor();

    SetBrightness();
}
