#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Display
 *  @{
 *  @defgroup Grid
 *  @brief Параметры сетки
 *  @{
 */

class Grid
{
public:
    int Left(void);

    int Right(void);

    int Width(void);

    float DeltaY(void);

    float DeltaX(void);

    int WidthInCells(void);

    int FullBottom(void);

    int FullHeight(void);

    int FullCenterHeight(void);

    int ChannelBottom(void);

    int ChannelHeight(void);

    int ChannelCenterHeight(void);

    int MathHeight(void);

    int MathBottom(void);

    int MathTop(void);

    int BottomMessages(void);

    void CoordTrigLevel(int *left, int *top, int width);
};


extern Grid grid;

/** @}  @}
 */
