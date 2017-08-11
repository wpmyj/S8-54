#include "defines.h"
#include "PainterText.h"
#include "Painter.h"
#include "Font/font.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"


#include <stdarg.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static TypeFont currentTypeFont = TypeFont_None;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Painter_SetFont(TypeFont typeFont)
{
    if (typeFont == currentTypeFont)
    {
        return;
    }
    uint8 command[4];
    command[0] = SET_FONT;
    command[1] = (uint8)typeFont;
    Painter_SendToDisplay(command, 4);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_LoadFont(TypeFont typeFont)
{
    /*
    const uchar *bytes = 0;
    if (typeFont == TypeFont_5)
    {
        bytes = font5display;
    }
    else if (typeFont == TypeFont_8)
    {
        bytes = font8display;
    }
    else if (typeFont == TypeFont_UGO)
    {
        bytes = fontUGOdisplay;
    }
    else if (typeFont == TypeFont_UGO2)
    {
        bytes = fontUGO2display;
    }
    */
    /*
    uint8 command[3084];
    command[0] = LOAD_FONT;
    command[1] = typeFont;
    for (int i = 0; i < 3080; i++)
    {
        command[2 + i] = bytes[i];
    }
    */
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ByteFontNotEmpty(int eChar, int byte)
{
    static const uint8 *bytes = 0;
    static int prevChar = -1;
    if (eChar != prevChar)
    {
        prevChar = eChar;
        bytes = font->symbol[prevChar].bytes;
    }
    return bytes[byte];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool BitInFontIsExist(int eChar, int numByte, int bit)
{
    static uint8 prevByte = 0;      /// \todo здесь точно статики нужны?
    static int prevChar = -1;
    static int prevNumByte = -1;
    if (prevNumByte != numByte || prevChar != eChar)
    {
        prevByte = font->symbol[eChar].bytes[numByte];
        prevChar = eChar;
        prevNumByte = numByte;
    }
    return prevByte & (1 << bit);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCharInColorDisplay(int eX, int eY, uchar symbol)
{
    int8 width = font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    Painter_SetPoint(x, y);
                }
                x++;
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawBigChar(int eX, int eY, int size, char symbol)
{
    int8 width = font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b * size + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    for (int i = 0; i < size; i++)
                    {
                        for (int j = 0; j < size; j++)
                        {
                            Painter_SetPoint(x + i, y + j);
                        }
                    }
                }
                x += size;
            }
        }
    }

    return eX + width * size;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCharHardCol(int x, int y, char symbol)
{
    char str[2] = {0, 0};
    str[0] = symbol;
    Painter_DrawText(x, y, str);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
extern void CalculateCurrentColor(void);


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawChar(int x, int y, char symbol)
{
    CalculateCurrentColor();
    if (Font_GetSize() == 5)
    {
        DrawCharHardCol(x, y + 3, symbol);
    }
    else if (Font_GetSize() == 8)
    {
        DrawCharHardCol(x, y, symbol);
    }
    else
    {
        DrawCharInColorDisplay(x, y, symbol);
    }
    return x + Font_GetLengthSymbol(symbol);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawCharC(int x, int y, char symbol, Color color)
{
    Painter_SetColor(color);
    return Painter_DrawChar(x, y, symbol);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawText(int x, int y, const char *text)
{
    if (*text == 0)
    {
        return x;
    }
    CalculateCurrentColor();

    int retValue = x;
    y += (8 - Font_GetSize());
#define SIZE_BUFFER 100
    uint8 command[SIZE_BUFFER];
    command[0] = DRAW_TEXT;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (int8)(y + 1);
    uint8 *pointer = command + 5;
    uint8 length = 0;

    int counter = 0;
    while (*text && length < (SIZE_BUFFER - 7))
    {
        *pointer = (uint8)(*text);
        retValue += Font_GetLengthSymbol(*text);
        text++;
        pointer++;
        length++;
        counter++;
    }

    *pointer = 0;
    *(command + 4) = length;
    int numBytes = ((length + 4) / 4) * 4 + 4;
    Painter_SendToDisplay(command, numBytes);
    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawTextC(int x, int y, const char *text, Color color)
{
    Painter_SetColor(color);
    return Painter_DrawText(x, y, text);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight)
{
    int8 width = font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    if ((x >= limitX) && (x <= (limitX + limitWidth)) && (y >= limitY) && (y <= limitY + limitHeight))
                    {
                        Painter_SetPoint(x, y);
                    }
                }
                x++;
            }
        }
    }

    return eX + width + 1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawTextWithLimitationC(int x, int y, const char *text, Color color, int limitX, int limitY, int limitWidth, int limitHeight)
{
    Painter_SetColor(color);
    int retValue = x;
    while (*text)
    {
        x = DrawCharWithLimitation(x, y, *text, limitX, limitY, limitWidth, limitHeight);
        retValue += Font_GetLengthSymbol(*text);
        text++;
    }
    return retValue + 1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsLetter(char symbol)
{
    static const bool isLetter[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
    };

    return isLetter[(uint8)symbol];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static char *GetWord(const char *firstSymbol, int *length, char buffer[20])
{
    int pointer = 0;
    *length = 0;

    while (IsLetter(*firstSymbol))
    {
        buffer[pointer] = *firstSymbol;
        pointer++;
        firstSymbol++;
        (*length)++;
    }
    buffer[pointer] = '\0';

    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsConsonant(char symbol)
{
    static const bool isConsonat[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false
    };

    return isConsonat[(uint8)symbol];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool CompareArrays(const bool *array1, const bool *array2, int numElems)
{
    for (int i = 0; i < numElems; i++)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }
    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Находит следующий перенос. C letters начинается часть слово, где нужно найти перенос, в lettersInSyllable будет записано число букв в найденном слоге. Если слово закончилось, функция возвращает false
static bool FindNextTransfer(char *letters, int8 *lettersInSyllable)
{

#define VOWEL       0   // Гласная
#define CONSONANT   1   // Согласная

    *lettersInSyllable = (int8)strlen(letters);
    if (strlen(letters) <= 3)
    {
        return false;
    }

    static const bool template1[3] = {false, true, true};               //     011     2   // После второго символа перенос
    static const bool template2[4] = {true, false, true, false};        //     1010    2
    static const bool template3[4] = {false, true, false, true};        //     0101    3
    static const bool template4[4] = {true, false, true, true};         //     1011    3
    static const bool template5[4] = {false, true, false, false};       //     0100    3
    static const bool template6[4] = {true, false, true, true};         //     1011    3
    static const bool template7[5] = {true, true, false, true, false};  //     11010   3
    static const bool template8[6] = {true, true, false, true, true};   //     11011   4

    bool consonant[20];

    int size = strlen(letters);
    for (int i = 0; i < size; i++)
    {
        consonant[i] = IsConsonant(letters[i]);
    }

    if (CompareArrays(template1, consonant, 3))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (CompareArrays(template2, consonant, 4))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (strlen(letters) < 5)
    {
        return false;
    }
    if (CompareArrays(template3, consonant, 4) || CompareArrays(template4, consonant, 4) || CompareArrays(template5, consonant, 4) || CompareArrays(template6, consonant, 4))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (strlen(letters) < 6)
    {
        return false;
    }
    if (CompareArrays(template7, consonant, 5))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (CompareArrays(template8, consonant, 5))
    {
        *lettersInSyllable = 4;
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8* BreakWord(char *word)
{
    int num = 0;
    static int8 lengthSyllables[10];
    char *position = word;
    while (FindNextTransfer(position, &(lengthSyllables[num])))
    {
        position += lengthSyllables[num];
        num++;
    }
    lengthSyllables[num + 1] = 0;
    if (strcmp(word, "структуру") == 0)
    {
        int8 lengths[] = {5, 2, 2, 0};
        memcpy(lengthSyllables, lengths, 4);
    }
    else if (strcmp(word, "соответствующей") == 0)
    {
        int8 lenghts[] = {4, 3, 4, 5, 3, 0};
        memcpy(lengthSyllables, lenghts, 6);
    }
    return lengthSyllables;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает часть слова до слога numSyllable(включительн) вместе со знаком переноса
static char* PartWordForTransfer(char *word, int8* lengthSyllables, int numSyllable, char buffer[30])
{
    int length = 0;
    for (int i = 0; i <= numSyllable; i++)
    {
        length += lengthSyllables[i];
    }
    memcpy((void*)buffer, (void*)word, length);
    buffer[length] = '-';
    buffer[length + 1] = '\0';
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Если draw == false, то рисовать символ не надо, фунция используется только для вычислений
static int DrawPartWord(char *word, int x, int y, int xRight, bool draw)
{
    int8 *lengthSyllables = BreakWord(word);
    int numSyllabels = 0;
    char buffer[30];
    for (int i = 0; i < 10; i++)
    {
        if (lengthSyllables[i] == 0)
        {
            numSyllabels = i;
            break;
        }
    }

    for (int i = numSyllabels - 2; i >= 0; i--)
    {
        char *subString = PartWordForTransfer(word, lengthSyllables, i, buffer);
        int length = Font_GetLengthText(subString);
        if (xRight - x > length - 5)
        {
            if (draw)
            {
                Painter_DrawText(x, y, subString);
            }
            return strlen(subString) - 1;
        }
    }

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawTextInRectWithTransfers(int eX, int eY, int eWidth, int eHeight, const char *text)
{
    int top = eY;
    int left = eX;
    int right = eX + eWidth;
    int bottom = eY + eHeight;

    char buffer[20];
    int numSymbols = strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < bottom && curSymbol < numSymbols)
    {
        while (x < right - 1 && curSymbol < numSymbols)
        {
            int length = 0;
            char *word = GetWord(text + curSymbol, &length, buffer);

            if (length <= 1)                            // Нет буквенных символов или один, т.е. слово не найдено
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x = Painter_DrawChar(x, y, symbol);
            }
            else                                            // А здесь найдено по крайней мере два буквенных символа, т.е. найдено слово
            {
                int lengthString = Font_GetLengthText(word);
                if (x + lengthString > right + 5)
                {
                    int numSymbols = DrawPartWord(word, x, y, right, true);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x = Painter_DrawText(x, y, word);
                }
            }
        }
        x = left;
        y += 9;
    }

    return y;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Возвращает высоту экрана, которую займёт текст text, при выводе от left до right в переменной height. Если bool == false, то текст не влезет на экран 
static bool GetHeightTextWithTransfers(int left, int top, int right, const char *text, int *height)
{
    char buffer[20];
    int numSymbols = strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < 231 && curSymbol < numSymbols)
    {
        while (x < right - 1 && curSymbol < numSymbols)
        {
            int length = 0;
            char *word = GetWord(text + curSymbol, &length, buffer);

            if (length <= 1)                            // Нет буквенных символов или один, т.е. слово не найдено
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x += Font_GetLengthSymbol(symbol);
            }
            else                                            // А здесь найдено по крайней мере два буквенных символа, т.е. найдено слово
            {
                int lengthString = Font_GetLengthText(word);
                if (x + lengthString > right + 5)
                {
                    int numSymbols = DrawPartWord(word, x, y, right, false);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x += Font_GetLengthText(word);
                }
            }
        }
        x = left;
        y += 9;
    }

    LIMITATION(*height, y - top + 4, 0, 239);

    return curSymbol == numSymbols;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawTextInBoundedRectWithTransfers(int x, int y, int width, const char *text, Color colorBackground, Color colorFill)
{
    int height = 0;
    GetHeightTextWithTransfers(x + 3, y + 3, x + width - 8, text, &height);

    Painter_DrawRectangleC(x, y, width, height, colorFill);
    Painter_FillRegionC(x + 1, y + 1, width - 2, height - 2, colorBackground);
    Painter_DrawTextInRectWithTransfersC(x + 3, y + 3, width - 8, height, text, colorFill);
    return y + height;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawTextInRectWithTransfersC(int x, int y, int width, int height, const char *text, Color color)
{
    Painter_SetColor(color);
    return Painter_DrawTextInRectWithTransfers(x, y, width, height, text);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawFormText(int x, int y, Color color, char *text, ...)
{
#define SIZE_BUFFER_DRAW_FORM_TEXT 200
    char buffer[SIZE_BUFFER_DRAW_FORM_TEXT];
    __va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);
    return Painter_DrawTextC(x, y, buffer, color);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawStringInCenterRect(int eX, int eY, int width, int eHeight, const char *text)
{
    int lenght = Font_GetLengthText(text);
    int height = Font_GetHeightSymbol(text[0]);
    int x = eX + (width - lenght) / 2;
    int y = eY + (eHeight - height) / 2;
    return Painter_DrawText(x, y, text);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawStringInCenterRectC(int x, int y, int width, int height, const char *text, Color color)
{
    Painter_SetColor(color);
    return Painter_DrawStringInCenterRect(x, y, width, height, text);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, const char *text, Color colorText, int widthBorder, Color colorBackground)
{
    int lenght = Font_GetLengthText(text);
    int eX = Painter_DrawStringInCenterRectC(x, y, width, height, text, colorBackground);
    int w = lenght + widthBorder * 2 - 2;
    int h = 7 + widthBorder * 2 - 1;
    Painter_FillRegion(eX - lenght - widthBorder, y - widthBorder + 1, w, h);
    Painter_DrawStringInCenterRectC(x, y, width, height, text, colorText);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter_DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, const char *text, Color colorBackground, Color colorFill)
{
    Painter_DrawRectangleC(x, y, width, height, colorFill);
    Painter_FillRegionC(x + 1, y + 1, width - 2, height - 2, colorBackground);
    Painter_SetColor(colorFill);
    return Painter_DrawStringInCenterRect(x, y, width, height, text);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int GetLenghtSubString(char *text)
{
    int retValue = 0;
    while (((*text) != ' ') && ((*text) != '\0'))
    {
        retValue += Font_GetLengthSymbol(*text);
        text++;
    }
    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int DrawSubString(int x, int y, char *text)
{
    int numSymbols = 0;
    while (((*text) != ' ') && ((*text) != '\0'))
    {
        x = Painter_DrawChar(x, y, *text);
        numSymbols++;
        text++;
    }
    return numSymbols;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int DrawSpaces(int x, int y, char *text, int *numSymbols)
{
    *numSymbols = 0;
    while (*text == ' ')
    {
        x = Painter_DrawChar(x, y, *text);
        text++;
        (*numSymbols)++;
    }
    return x;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawTextInRect(int x, int y, int width, char *text)
{
    int xStart = x;
    int xEnd = xStart + width;

    while (*text != 0)
    {
        int length = GetLenghtSubString(text);
        if (length + x > xEnd)
        {
            x = xStart;
            y += Font_GetHeightSymbol(*text);
        }
        int numSymbols = 0;
        numSymbols = DrawSubString(x, y, text);
        text += numSymbols;
        x += length;
        x = DrawSpaces(x, y, text, &numSymbols);
        text += numSymbols;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawTextRelativelyRight(int xRight, int y, const char *text)
{
    int lenght = Font_GetLengthText(text);
    Painter_DrawText(xRight - lenght, y, text);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color color1, Color color2)
{
    Painter_DrawCharC(x, y, symbol1, color1);
    Painter_DrawCharC(x, y, symbol2, color2);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_Draw4SymbolsInRect(int x, int y, char eChar)
{
    for (char i = 0; i < 2; i++)
    {
        Painter_DrawChar(x + 8 * i, y, eChar + i);
        Painter_DrawChar(x + 8 * i, y + 8, eChar + i + 16);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_Draw10SymbolsInRect(int x, int y, char eChar)
{
    for (char i = 0; i < 5; i++)
    {
        Painter_DrawChar(x + 8 * i, y, eChar + i);
        Painter_DrawChar(x + 8 * i, y + 8, eChar + i + 16);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawBigText(int eX, int eY, int size, const char *text)
{
    int numSymbols = strlen(text);

    int x = eX;

    for (int i = 0; i < numSymbols; i++)
    {
        x = Painter_DrawBigChar(x, eY, size, text[i]);
        x += size;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int Painter_DrawBigCharInBuffer(int eX, int eY, int size, char symbol, uint8 buffer[320][240])
{
    int8 width = font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b * size + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    for (int i = 0; i < size; i++)
                    {
                        for (int j = 0; j < size; j++)
                        {
                            int fullX = x + i;
                            int fullY = y + j;

                            if (fullX >= 0 && fullX < 320 && fullY >= 0 && fullY < 240)
                            {
                                buffer[fullX][fullY] = 1;
                            }
                        }
                    }
                }
                x += size;
            }
        }
    }

    return eX + width * size;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawBigTextInBuffer(int eX, int eY, int size, const char *text, uint8 buffer[320][240])
{
    for (int x = 0; x < 320; x++)
    {
        for (int y = 0; y < 240; y++)
        {
            buffer[x][y] = 0;
        }
    }

    int numSymbols = strlen(text);

    int x = eX;

    for (int i = 0; i < numSymbols; i++)
    {
        x = Painter_DrawBigCharInBuffer(x, eY, size, text[i], buffer);
        x += size;
    }
}
