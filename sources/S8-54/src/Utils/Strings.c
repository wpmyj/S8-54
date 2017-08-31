// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Strings.h"
#include <ctype.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool ChooseSymbols(const uint8 **string);    // Возвращает false, если выбор невозможен - строка кончилась.
static bool ChooseSpaces(const uint8 **string);     // Возвращает false, если выбор невозможен - строка кончилась.


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GetNumWordsInString(const uint8 *string)
{
    ChooseSpaces(&string);

    volatile bool run = true;
    while (run)
    {
        int numWords = 0;
        
        if (ChooseSymbols(&string))
        {
            numWords++;
        }
        else
        {
            return numWords;
        }
        ChooseSpaces(&string);
    }

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool GetWord(const uint8 *string, Word *word, const int numWord)
{
    ChooseSpaces(&string);

    int currentWord = 0;

    volatile bool run = true;
    while (run)
    {
        if (currentWord == numWord)
        {
            word->address = (uint8 *)string;
            ChooseSymbols(&string);
            word->numSymbols = (int8)(string - word->address);
            
            uint8 *pointer = word->address;
            int numSymbols = word->numSymbols;
            for (int i = 0; i < numSymbols; i++)
            {
                *pointer = (uint8)toupper(*pointer);
                pointer++;
            }
            return true;
        }
        if (ChooseSymbols(&string))
        {
            currentWord++;
        }
        else
        {
            return false;
        }
        ChooseSpaces(&string);
    }

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool WordEqualZeroString(Word *word, char* string)
{
    char *ch = string;
    char *w = (char*)(word->address);

    while (*ch != 0)
    {
        if (*ch++ != *w++)
        {
            return false;
        }
    }

    return (ch - string) == word->numSymbols;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
#define  SYMBOL(x) (*(*(x)))


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ChooseSymbols(const uint8 **string)
{
    if (SYMBOL(string) == 0x0d && SYMBOL(string + 1) == 0x0a)
    {
        return false;
    }

    while (SYMBOL(string) != ' ' && SYMBOL(string) != 0x0d && SYMBOL(string + 1) != 0x0a)
    {
        (*string)++;
    }

    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ChooseSpaces(const uint8 **string)
{
    if (SYMBOL(string) == 0x0d && SYMBOL(string + 1) == 0x0a)
    {
        return false;
    }

    while (SYMBOL(string) == ' ')
    {
        (*string)++;
    }

    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
#undef SYMBOL


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EqualsStrings(char *str1, char *str2)
{
    int size = strlen(str1);

    for (int i = 0; i < size; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EqualsZeroStrings(char *str1, char *str2)
{
    while ((*str1) == (*str2))
    {
        if ((*str1) == '\0')
        {
            return true;
        }
        str1++;
        str2++;
    }
    return false;
}
