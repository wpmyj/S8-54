#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** @addtogroup Painter
 *  @{
 *  @defgroup PainterData
 *  @brief Функции отрисовки данных
 *  @{
 */


/// Отрисовка сигнала на основном экране.
void PainterDataNew_DrawData(void);
/// Прервать процесс рисования
void PainterDataNew_InterruptDrawing(void);
/// Нарисовать результат математической функции
void PainterData_DrawMath(void);
/// Нарисовать данные в окне памяти
void PainterData_DrawMemoryWindow(void);


/** @}  @}
 */
