#include <Utils/Log.h>

#include <iostream>

namespace scaffold
{
    void Log(LogLevel level, std::string_view message)
    {
        if (level <= g_LogLevel)
        {
            std::format_to(
                std::ostream_iterator<char>(std::cout),
                "[{0}] {1}\n",
                level, message
            );
        }
    }
}