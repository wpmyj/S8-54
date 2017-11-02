#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** @addtogroup Painter
 *  @{
 *  @defgroup PainterData
 *  @brief Функции отрисовки данных
 *  @{
 */


class PainterData
{
public:
    /// Отрисовка сигнала на основном экране.
    void DrawData(void);
    /// Прервать процесс рисования
    void InterruptDrawing(void);
    /// Нарисовать результат математической функции
    void DrawMath(void);
};


extern PainterData painterData;


/** @}  @}
 */
