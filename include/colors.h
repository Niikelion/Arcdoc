#ifndef COLORS_H_INCLUDED
#define COLORS_H_INCLUDED

#if defined(_WIN32) || defined(_WIN64)
#   define CMD_COLOR_OS_WINDOWS
#elif defined(__APPLE__)
#   define CMD_COLOR_OS_MACOS
#elif defined(__unix__) || defined(__unix)
#   define CMD_COLOR_OS_LINUX
#else
#   error unsupported platform
#endif

#include <iostream>
#ifdef CMD_COLOR_OS_WINDOWS
#include <windows.h>
#endif // CMD_COLOR_WINDOWS

namespace CmdColors
{
    enum class Colors
    {
        Default,
        Red,
        Yellow
    };
    namespace _details
    {
        void setColor(Colors color)
        {
        #ifdef CMD_COLOR_OS_WINDOWS
            thread_local int defColor = -1;
            HANDLE  hConsole;
            int k;
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO info;
            int back = 0;
            if (GetConsoleScreenBufferInfo(hConsole, &info))
            {
                back = info.wAttributes&0xF0;
                if (defColor == -1)
                    defColor = info.wAttributes&0x0F;
            }

            switch(color)
            {
            case Colors::Red:
                {
                    k = FOREGROUND_INTENSITY | FOREGROUND_RED;
                    break;
                }
            case Colors::Yellow:
                {
                    k = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
                    break;
                }
            case Colors::Default:
                {
                    k = defColor;
                    break;
                }
            default:
                k = 0;
            }
            SetConsoleTextAttribute(hConsole, k|back);
        #elif defined(CMD_COLOR_OS_LINUX) || defined(CMD_COLOR_OS_MACOS)
            switch(color)
            {
            case Colors::Red:
                {
                    std::cout << "\033[38;5;31m";
                    break;
                }
            case Colors::Default:
                {
                    std::cout << "\033[0m";
                    break;
                }
            default:
            }
        #endif
        }
    }
    using namespace std;
    inline ostream& red(ostream& o)
    {
        _details::setColor(Colors::Red);
        return o;
    }
    inline ostream& yellow(ostream& o)
    {
        _details::setColor(Colors::Yellow);
        return o;
    }
    inline ostream& none(ostream& o)
    {
        _details::setColor(Colors::Default);
        return o;
    }
}

#endif // COLORS_H_INCLUDED
