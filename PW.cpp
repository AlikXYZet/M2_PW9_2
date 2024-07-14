#include <iostream>
#include <string>

#include <conio.h>  // Для неявного ввода данных: "_getch()"

#include <chrono>   // Задержка времени

#include <thread>   // Управление потоками
#include <mutex>    // Очередь вывода на консоль для потоков

#include <windows.h>// Для скрытия курсора и обработки нажатий
#include <vector>   // Для массива препятствий



/* ---   Глабальные переменные   --- */

// Организация очереди вывода на консоль
std::mutex mtx_Print;

// Флаг выхода из цикла во всех потоках и завершение программы
bool bNotEnd = true;
//---------------------------------------------------------------------



/* ---   Классы и структуры   --- */

// Препятствие
class Obstacle
{
public:
    /*   Конструкторы   */
    Obstacle(int in_location = 120) : location(in_location)
    {
        // Использовать текущее время в качестве начального значения для генератора псевдослучайных чисел
        srand(time(0));

        type = rand() % 6;
        restructure();
    };
    //----------------------------------------



    /*   Перегрузка декремента   */
    // Префиксный
    Obstacle& operator--()
    {
        --location;
        return *this;
    };
    // Постфиксный
    Obstacle operator--(int)
    {
        Obstacle buf = *this;
        --location;
        return buf;
    };
    //----------------------------------------



    /*   Доступные функции   */
    // Отрисовка в коонсоль
    void outPrint(const int& in_lowerPosition)
    {
        mtx_Print.lock();
        {
            switch (type)
            {
            case 0: // Одиночный кактус
                printf("\x1B[%d;%df", in_lowerPosition, location);
                std::cout << "# ";
                break;

            case 1: // Двойной кактус
                printf("\x1B[%d;%df", in_lowerPosition, location);
                std::cout << "## ";
                break;

            case 2: // Выысокий кактус
                printf("\x1B[%d;%df", in_lowerPosition, location);
                std::cout << "# ";
                printf("\x1B[%d;%df", in_lowerPosition - 1, location);
                std::cout << "# ";
                break;

            case 3: // Птица
            case 4:
            case 5:
                printf("\x1B[%d;%df", in_lowerPosition - height, location);
                std::cout << "V ";
                break;
            }

            // Debug:
            //printf("\x1B[%d;%df", 20, location);
            //std::cout << int(type) << " ";
        }
        mtx_Print.unlock();
    }
    // Очистка отображения
    void clear(const int& in_lowerPosition)
    {
        if (length > 1)
        {
            mtx_Print.lock();
            {
                printf("\x1B[%d;%df", in_lowerPosition - 1, location);
                std::cout << " ";
            }
            mtx_Print.unlock();
        }

        mtx_Print.lock();
        {
            for (uint8_t i = 0; i < width; ++i)
            {
                printf("\x1B[%d;%df", in_lowerPosition - height, location - i);
                std::cout << " ";
            }
        }
        mtx_Print.unlock();
    }
    //----------------------------------------



    /*   Доступные переменные   */
    uint8_t type;   // Тип
    uint8_t length; // Длина
    uint8_t width;  // Ширина
    uint8_t height; // На какой высоте
    int location = 120;   // Местоположение
    //----------------------------------------

private:

    // Реструктурировать данные по типу
    void restructure()
    {
        switch (type)
        {
        case 0:
            length = 1;
            width = 1;
            height = 0;
            break;

        case 1:
            length = 1;
            width = 2;
            height = 0;
            break;

        case 2:
            length = 2;
            width = 1;
            height = 0;
            break;

        case 3:
            length = 1;
            width = 1;
            height = 0;
            break;

        case 4:
            length = 1;
            width = 1;
            height = 1;
            break;

        case 5:
            length = 1;
            width = 1;
            height = 2;
            break;
        }
    }
};
//---------------------------------------------------------------------



/* ---   Потоковые функции   --- */

// Получение команд игрока
void tf_GameInput(int8_t& out_Command)
{
    int curInput = 0;

    HANDLE s_in = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dr;
    INPUT_RECORD rec;

    while (bNotEnd)
    {
        // Ожидание ввода команды
        ReadConsoleInput(s_in, &rec, 1, &dr);

        if (rec.EventType == KEY_EVENT)
        {
            if (rec.Event.KeyEvent.bKeyDown)
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
                    bNotEnd = false;
                    break;
                }
            else
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

    // Debug:
    //printf("\x1B[%d;%df", 20, 0);
    //std::cout << "End: GameInput";
}

// Рендер графики
void tf_GraphicsRendering(const int8_t& in_Command, std::vector<Obstacle>& in_obstacles)
{
    unsigned int curPoints = 0;
    uint16_t time = 1000;
    unsigned int obstaclesCycle = 0;
    int lowerBorder = 7;

    mtx_Print.lock();
    {
        std::cout << "Кол. очков: ";

        printf("\x1B[%d;%df", 3, 0);
        std::cout
            << "----------------------------------------------------------------------------------------------------------------------" << '\n'   // 3
            << "                                                                                                                      " << '\n'   // 4
            << "                                                                                                                      " << '\n'   // 5
            << "                                                                                                                      " << '\n'   // 6
            << "                                                                                                                      " << '\n'   // 7
            << "----------------------------------------------------------------------------------------------------------------------" << '\n'   // 8
            << std::endl;
    }
    mtx_Print.unlock();



    while (bNotEnd)
    {
        /* ---   Отрисовка   --- */

        // Количество очков:
        ++curPoints;

        mtx_Print.lock();
        {
            printf("\x1B[%d;%df", 1, 13);
            std::cout << curPoints;
        }
        mtx_Print.unlock();

        // Препятствия
        if (0 == obstaclesCycle--)
        {
            in_obstacles.push_back(Obstacle(119));
            obstaclesCycle = 50;
        }

        for (Obstacle& curObstacle : in_obstacles)
        {
            --curObstacle;
            curObstacle.outPrint(lowerBorder);
        }

        if (in_obstacles.data() && in_obstacles.data()->location <= 0)
        {
            in_obstacles.data()->clear(lowerBorder);
            for (std::vector<Obstacle>::iterator i = in_obstacles.begin(); i < (in_obstacles.end() - 1); ++i)
            {
                *i = *(i + 1);
            }
            in_obstacles.pop_back();
        }
        //----------------------------------------



        // Задержка времени: имитация скорости бега
        if (time > 10)
            time = 100 - (curPoints / 8);
        else
            time = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

    // Debug:
    //printf("\x1B[%d;%df", 21, 0);
    //std::cout << "End: GraphicsRendering";
}

// Отображение персонажа
void tf_DinoRendering(const int8_t& in_Command, uint8_t& out_Position)
{
    int upperBorder = 4;
    int lowerBorder = out_Position = 7;
    uint8_t bFall = 0;
    uint8_t prePosition = 6;

    while (bNotEnd)
    {
        // Обработка команд
        switch (in_Command)
        {
        case 1:
            if (out_Position == lowerBorder)    // Короткий прыжок
            {
                --out_Position;
                bFall = 1;
            }
            else if (bFall == 1)    // Длинный прыжок
            {
                --out_Position;
                bFall = 2;
            }
            else if (bFall == 2)    // Задержка в воздухе
            {
                bFall = 0;
            }
            else                    // Падение
            {
                ++out_Position;
            }

            break;

        case 2:
            out_Position = lowerBorder; // Резкое приземление

            break;

        default:
            if (bFall)                              // Задержка в воздухе
                bFall = 0;
            else if (out_Position != lowerBorder)   // Падение
                ++out_Position;

            break;
        }



        /* ---   Отрисовка   --- */

        // Дино:
        if (prePosition != out_Position)
        {

            mtx_Print.lock();
            {
                printf("\x1B[%d;%df", int(prePosition), 3);
                std::cout << " " << std::endl;

                printf("\x1B[%d;%df", int(out_Position), 3);
                std::cout << "$" << std::endl;
            }
            mtx_Print.unlock();
            prePosition = out_Position;
        }
        //----------------------------------------

        // Скорость отображения персонажа
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Debug:
    //printf("\x1B[%d;%df", 22, 0);
    //std::cout << "End: DinoRendering";
}

// Проверка препятствий
void tf_CheckingObstacles(int8_t& out_Command, const uint8_t& in_Position, const std::vector<Obstacle>& in_obstacles)
{
    int lowerBorder = 7;
    uint8_t bufCheackPosition;
    int bufLocation;

    // Ожидание первого препятствия
    while (!(in_obstacles.data())) {}

    while (bNotEnd)
    {
        bufLocation = in_obstacles.data()->location;
        if (bufLocation == 3
            || bufLocation + in_obstacles.data()->width - 1 == 3)
        {
            bufCheackPosition = lowerBorder - in_obstacles.data()->height;

            if (bufCheackPosition == in_Position
                || bufCheackPosition + in_obstacles.data()->length - 1 == in_Position)
            {
                bNotEnd = false;
            }
        }
    }

    // Debug:
    //printf("\x1B[%d;%df", 23, 0);
    //std::cout << "End: CheckingObstacles";
}
//---------------------------------------------------------------------



int main()
{
    /* ---   Предварительное   --- */

    // Локализация
    setlocale(LC_ALL, "Russian");

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

    // Массив препятствий
    std::vector<Obstacle> obstacles;
    //----------------------------------------



    /* ---   Потоки   --- */

    // Ввод данных
    std::thread t_GameInput(tf_GameInput, std::ref(curCommand));
    t_GameInput.detach();   // По окончании программы, игнорируем данный поток

    // Рендер графики
    std::thread t_GraphicsRendering(tf_GraphicsRendering, std::ref(curCommand), std::ref(obstacles));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Отображение персонажа
    std::thread t_Dino(tf_DinoRendering, std::ref(curCommand), std::ref(curPosition));

    // Проверка препятствий
    std::thread t_CheckingObstacles(tf_CheckingObstacles, std::ref(curCommand), std::ref(curPosition), std::ref(obstacles));
    //----------------------------------------



    // Бесконечный цикл ожидания "Esc" или проигрыша
    while (bNotEnd) {}

    // Очистка потоков
    t_GraphicsRendering.join();
    t_Dino.join();
    t_CheckingObstacles.join();

    printf("\x1B[%d;%df", curPosition, 3);
    std::cout << 0 << std::endl;

    printf("\x1B[%d;%df", 10, 0);
    system("pause");

    return 0;
}