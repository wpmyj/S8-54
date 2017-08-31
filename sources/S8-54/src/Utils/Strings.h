#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
 *  @{
 *  @defgroup Strings
 *  @{
 */

typedef struct
{
    uint8 *address;
    int8   numSymbols;
} Word;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Возвращает количество слов в строке. Разделители - пробелы. Строка заканчивается байтами 0x0d, 0x0a
int GetNumWordsInString(const uint8 *string);
/// Эта команда сразу преобразует к верхенму регистру слово
bool GetWord(const uint8 *string, Word *word, const int numWord);

bool WordEqualZeroString(Word *word, char *string);
/// Сравнивает две строки. Число символов берётся из str1
bool EqualsStrings(char *str1, char *str2);

bool EqualsZeroStrings(char *str1, char *str2);

/** @}  @}
 */
