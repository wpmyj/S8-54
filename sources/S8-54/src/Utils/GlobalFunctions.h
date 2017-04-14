#pragma once
#include "Settings/DataSettings.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Utils
 *  @{
 *  @defgroup GlobalFunctions Global Functions
 *  @{
 */

/// Выделяет память из кучи для канала. Настройки длины памяти берёт из ds или set (если ds == 0)
void* AllocMemForChannelFromHeap(Channel ch, DataSettings *ds);

int RequestBytesForChannel(Channel ch, DataSettings *ds);

char* FloatFract2String(float value, bool alwaysSign, char bufferOut[20]);
/// @brief Преобразует value типа float в текстовую строку
/// @attention Строка будет храниться до следующего вызова функции. Если результат нужен большее количество времени, то его нужно скопироавать себе
/// @retval указатель на строку с числом
char* Float2String(float value,                 ///< значение
                   bool alwaysSign,             ///< если установлен в true, перед числом всегда будет знак, если установлен в false, только если число отрицательное
                   int numDigits,               ///< число разрядов мантиссы
                   char bufferOut[20]           ///< сюда записывается возвращаемое значение
                   );
/// @brief Преобразует value в текстовую строку. При этом выводятся все 8 разрядов
/// @attention Строка будет храниться до следующего вызова функции. Если результат нужен большее количество времени, то его нужно скопировать себе
char* Bin2String(uint8 value, char bufferOut[9]);
/// @brief Преобразует value в текстовую строку
/// @attention Строка будте храниться до следующего вызова функции. Если рузультат нужен большее количество времени, то его нужно скопировать себе
char* Bin2String16(uint16 value, char bufferOut[19]);
/// @brief Преобразует value в текстовую строку в шестнадцатиричном виде
/// @attention Строка будет храниться до следующего вызова функции. Если результат нужен большее количество времени, то его нужно скопировать себе
char* Hex8toString(uint8 value, char bufferOut[3], bool upper);

char* Hex16toString(uint16 value, char bufferOut[5], bool upper);

char* Hex32toString(uint32 value, char bufferOut[9], bool upper);
/// @brief Преобразует value в текстовую строку
/// @attention Строка будет храниться до следующего вызова функции. Если результат нужен большее количество времени, то его нужно скопировать себе
char* Int2String(int value,                     ///< значение
                 bool alwaysSign,               ///< если установлен в true, знак будет выводиться всегда
                 int numMinFields,              ///< минимальное число занимаемых знакомест. Если для вывода числа столько не требуется, лишние заполняются нулями
                 char bufferOut[20]             ///< сюда записывается возвращаемое значение
                 );

bool String2Int(char *str, int *value);
/// @brief Преобразует voltage вольт в текстовую строку
/// @attention Страка будет храниться до следующего вызова функции. Если результат нужен большее количество времени, то его нужно скопитровать себе
char* Voltage2String(float voltage,             ///< значение напряжения в вольтах
                     bool alwaysSign,           ///< если установлено в true, знак выводится всегда
                     char bufferOut[20]         ///< сюда записывается возвращаемое значение
                     );
/// @brief Преобразует time секунд в текстовую строку
char* Time2String(float time,                   ///< время в секундах. Может быть отрицательным
                  bool alwaysSign,              ///< если true, знак показывается даже если time > 0
                  char bufferOut[20]);          ///< сюда записывается возвращаемое значение
/// @brief Преобразует time секунд в текстовую строку
char* Time2StringAccuracy(float time,           ///< время в секундах. Может быть отрицательным
                          bool alwaysSign,      ///< если true, знак показывается даже если time > 0
                          char bufferOut[20],   ///< сюда записывается возвращаемое значение
                          int numDigits);       ///< количество цифр в строке
/// Преобразует freq герц в текстовую строку
char* Freq2String(float freq, bool, char bufferOut[20]);
/// Преобразует freq герц в текстовую строку. При этом число количество цифр в строке равно numDigits
char* Freq2StringAccuracy(float freq, char bufferOut[20], int numDigits);
/// Преобразует градусы в строку
char* Phase2String(float phase, bool, char bufferOut[20]);
/// Преобразует децибелы в текстовую строку
char* Float2Db(float value, int numDigits, char bufferOut[20]);
/// Возвращает true, если value входит в диапазон [min; max]
bool IntInRange(int value,                      ///< проверяемое значение
                int min,                        ///< нижняя граница диапазона
                int max                         ///< верхняя граница диапазона
                );

int BCD2Int(uint bcd);

void BubbleSortINT(int array[], int num);
/// Возвращает максимальное значение из трёх
float MaxFloat(float val1, float val2, float val3);
/// Увеличивает значение по адресу val на 1. Затем, если результат превышает max, приравинвает его к min
int8 CircleIncreaseInt8(int8 *val, int8 min, int8 max);

int16 CircleIncreaseInt16(int16 *val, int16 min, int16 max);
/// Умеьшает значение по адресу val на 1. Затем, если результат меньше min, приравнивает его max
int8 CircleDecreaseInt8(int8 *val, int8 min, int8 max);

int16 CircleDecreaseInt16(int16 *val, int16 min, int16 max);

int CircleIncreaseInt(int *val, int min, int max);

int CircleDecreaseInt(int *val, int min, int max);
/// Увелечивает значение по адресу val на delta. Затем, если результат больше max, приравнивает его min
float CircleAddFloat(float *val, float delta, float min, float max);
/// Уменьшает значение по адресу val на delta. Затем, елси результат меньше min, приравнивает его masx
float CircleSubFloat(float *val, float delta, float min, float max);
/// Прибавить к значению по адресу val delta. Если результат не вписывается в диапазон [min; max], ограничить
void AddLimitationFloat(float *val,             ///< по этому адресу хранится значение.
                        float delta,            ///< а это дельта.
                        float min,              ///< нижняя граница диапазона.
                        float max               ///< верхняя граница диапазона.
                        );
/// Обменивает местами содержимое памяти по адресам value0 и value1
void SwapInt(int *value0, int *value1);
/// Размещает переменные value0 и value1 в порядке возрастания
void SortInt(int *value0, int *value1);
/// Возвращает изображение регулятора, соответствующее его текущему положению
char GetSymbolForGovernor(int value);
/// Функция-заглушка. Ничего не делает
void EmptyFuncVV(void);
/// Функция-заглушка. Ничего не делает
void EmptyFuncVpV(void *);
/// Функция-заглушка. Ничего не делает
void EmptyFuncVII(int, int);
/// Функция-заглушка. Ничего не делает
void EmptyFuncpVII(void *, int, int);

char* IntToStrCat(char *_buffer, int _value);

/** @}  @}
 */
