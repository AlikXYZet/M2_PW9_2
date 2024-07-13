#include <iostream>
#include <string>

#include <conio.h>  // Для неявного ввода данных: "_getch()"

#include <chrono>   // Задержка времени

#include <thread>   // Управление потоками
#include <mutex>    // Очерёдность потоков

#include <windows.h>// Для скрытия курсора и обработки нажатий



/* ---   Потоковые функции   --- */

// Получение команд игрока
void tf_GameInput(int8_t& out_Command)
{
    int curInput = 0;

    HANDLE s_in = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dr;
    INPUT_RECORD rec;

    while (true)
    {
        ReadConsoleInput(s_in, &rec, 1, &dr);

        if (rec.EventType == KEY_EVENT)
            if (rec.Event.KeyEvent.bKeyDown)
            {
                switch (rec.Event.KeyEvent.wVirtualKeyCode)
                {
                    // Клавиши "прыжок":
                case VK_UP:     // Вверх
                case VK_NUMPAD8:// Num 8
                case 0x57:      // W
                    out_Command = 1;
                    break;

                    // Клавиши "пригнуться":
                case VK_DOWN:   // Вниз
                case VK_NUMPAD2:// Num 2
                case 0x53:      // S
                    out_Command = 2;
                    break;

                    // Клавиши "выход":
                case VK_ESCAPE:
                    out_Command = 0;
                    break;
                }
            }
            else
            {
                switch (out_Command)
                {
                case 1:
                    switch (rec.Event.KeyEvent.wVirtualKeyCode)
                    {
                        // Клавиши "прыжок":
                    case VK_UP:     // Вверх
                    case VK_NUMPAD8:// Num 8
                    case 0x57:      // W
                        out_Command = -1;
                        break;
                    }
                    break;

                case 2:
                    switch (rec.Event.KeyEvent.wVirtualKeyCode)
                    {
                        // Клавиши "прыжок":
                    case VK_DOWN:   // Вниз
                    case VK_NUMPAD2:// Num 2
                    case 0x53:      // S
                        out_Command = -1;
                        break;
                    }
                    break;
                }
            }
    }
}

// Рендер графики
void tf_GraphicsRendering(int8_t& in_Command, uint8_t& out_Position)
{
    unsigned int curPoints = 0;
    uint16_t time = 1000;
    int upperBorder = 4;
    int lowerBorder = out_Position = 7;
    bool bFall = false;



    std::cout << "Кол. очков: ";

    printf("\x1B[%d;%df", upperBorder - 1, 0);

    std::cout
        << "------------------------------------------------------------------------------------------------------------------------" << '\n'   // 3
        << "                                                                                                                        " << '\n'   // 4
        << "                                                                                                                        " << '\n'   // 5
        << "                                                                                                                        " << '\n'   // 6
        << "                                                                                                                        " << '\n'   // 7
        << "------------------------------------------------------------------------------------------------------------------------" << '\n'   // 8
        << std::endl;



    while (in_Command)
    {
        // Обработка команд
        switch (in_Command)
        {
        case 1:

            if (out_Position == lowerBorder)
            {
                --out_Position;
                bFall = true;
            }
            else if (bFall)
            {
                --out_Position;
                bFall = false;
            }
            else
            {
                ++out_Position;
            }

            break;
        case 2:
            out_Position = lowerBorder;

            break;

        default:
            if (out_Position == lowerBorder)
                bFall = false;
            else
                ++out_Position;

            break;
        }



        /* ---   Отрисовка   --- */

        // Количество очков:
        ++curPoints;
        printf("\x1B[%d;%df", 1, 13);
        std::cout << curPoints << std::endl;

        // Дино:
        for (int i = upperBorder; i <= lowerBorder; ++i)
        {
            printf("\x1B[%d;%df", i, 3);
            std::cout << " ";
        }

        printf("\x1B[%d;%df", int(out_Position), 3);
        std::cout << "$" << std::endl;
        //----------------------------------------



        // Задержка времени: имитация скорости бега
        if (time > 50)
            time = 100 - (curPoints);
        else
            time = 50;
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
}

// Проверка препятствий
void tf_CheckingObstacles(const uint8_t& out_Position)
{
    while (true)
    {

    }
}
//---------------------------------------------------------------------



int main()
{
    /* ---   Предварительное   --- */

    // Локализация
    setlocale(LC_ALL, "Russian");

    // Использовать текущее время в качестве начального значения для генератора псевдослучайных чисел
    srand(time(0));

    // Скрытие курсора консоли
    auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(handle, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &structCursorInfo);
    //----------------------------------------



    /* ---   Инициализация переменных   --- */

    // Текущая комманда
    int8_t curCommand = -1;

    // Текущая позиция
    uint8_t curPosition = 1;
    //----------------------------------------



    /* ---   Потоки   --- */

    // Ввод данных
    std::thread t_GameInput = std::thread(tf_GameInput, std::ref(curCommand));
    t_GameInput.detach();

    // Рендер графики
    std::thread t_GraphicsRendering = std::thread(tf_GraphicsRendering, std::ref(curCommand), std::ref(curPosition));
    t_GraphicsRendering.detach();

    // Проверка препятствий
    std::thread t_CheckingObstacles = std::thread(tf_CheckingObstacles, std::ref(curPosition));
    t_CheckingObstacles.detach();
    //----------------------------------------



    // Бесконечный цикл ожидания "Esc"
    while (curCommand != 0) {}
    curPosition = 0;



    printf("\x1B[%d;%df", 10, 0);
    system("pause");

    return 0;
}