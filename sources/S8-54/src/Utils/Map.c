

#include "defines.h"
#include "Strings.h"
#include "Map.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8 GetValueFromMap(const MapElement *map, Word *eKey)
{
    int numKey = 0;
    char *key = map[numKey].key;
    while (key != 0)
    {
        if (WordEqualZeroString(eKey, key))
        {
            return map[numKey].value;
        }
        numKey++;
        key = map[numKey].key;
    }
    return 255;
}
