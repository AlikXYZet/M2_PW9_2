#include <iostream>
#include <string>

#include <conio.h>  // Для неявного ввода данных: "_getch()"

#include <chrono>   // Задержка времени

#include <thread>   // Управление потоками
#include <mutex>    // Очерёдность потоков



/* ---   Потоковые функции   --- */

// Получение команд игрока
void tf_GameInput(int8_t& out_Command)
{
    int curInput = 0;
    int8_t preCommand = 0;

    while (true)
    {
        curInput = _getch();

        // Если вводимая клавиша с двойным кодом
        if (curInput == 0 || curInput == 224)
            curInput = _getch();    // то взять второй код

        switch (curInput)
        {
            // Клавиши "прыжок":
        case 32:    // Пробел
        case 56:    // 8
        case 72:    // Вверх
        case 87:    // W
        case 119:   // w
        case 150:   // Ц
        case 230:   // ц

            if (preCommand == 1)
                out_Command = 11;
            else
                out_Command = 1;

            break;

            // Клавиши "пригнуться":
        case 50:    // 2
        case 80:    // Вниз
        case 83:    // S
        case 115:   // s
        case 155:   // Ы
        case 235:   // ы

                out_Command = 2;

            break;

            // Клавиши "выход":
        case 27:    // Esc
            out_Command = 0;
            break;
        }

        // Тест
        //std::cout << (int)out_Command << '\n';

        preCommand = out_Command;
    }
}

// Рендер графики
void tf_GraphicsRendering()
{

}

// Проверка препятствий
void tf_CheckingObstacles()
{

}
//---------------------------------------------------------------------



int main()
{
    /* ---   Предварительное   --- */

    // Локализация
    setlocale(LC_ALL, "Russian");

    // Использовать текущее время в качестве начального значения для генератора псевдослучайных чисел
    srand(time(0));
    //----------------------------------------



    /* ---   Инициализация переменных   --- */

    // Текущая комманда
    int8_t curCommand = -1;
    //----------------------------------------



    /* ---   Потоки   --- */

    // Ввод данных
    std::thread t_GameInput = std::thread(tf_GameInput, std::ref(curCommand));

    // Рендер графики
    std::thread t_GraphicsRendering = std::thread(tf_GraphicsRendering);

    // Проверка препятствий
    std::thread t_CheckingObstacles = std::thread(tf_CheckingObstacles);
    //----------------------------------------



    // Бесконечный цикл ожидания "Esc"
    while (curCommand != 0) {}



    // Принудительное закрытие потоков
    t_GameInput.detach();
    t_GraphicsRendering.detach();
    t_CheckingObstacles.detach();

    return 0;
}