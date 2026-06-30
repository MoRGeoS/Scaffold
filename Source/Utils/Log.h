#pragma once

#include <string_view>
#include <format>

namespace scaffold
{
    enum class LogLevel
    {
        Error,
        Warn,
        Info,
        Debug
    };

    inline LogLevel g_LogLevel = LogLevel::Info;
    void Log(LogLevel level, std::string_view message);
}

#define SCFLD_LOG(level, message) ::scaffold::Log(::scaffold::LogLevel::level, message)

#define SCFLD_LOG_ERROR(message) SCFLD_LOG(Error, message)
#define SCFLD_LOG_WARN(message)  SCFLD_LOG(Warn,  message)
#define SCFLD_LOG_INFO(message)  SCFLD_LOG(Info,  message)
#define SCFLD_LOG_DEBUG(message) SCFLD_LOG(Debug, message)

namespace std
{
    template <>
    struct formatter<scaffold::LogLevel>
    {
        constexpr auto parse(format_parse_context& ctx)
        {
            return ctx.begin();
        }

        auto format(scaffold::LogLevel level, format_context& ctx) const
        {
            std::string_view str = "Unknown";

            switch (level)
            {
                case ::scaffold::LogLevel::Error: str = "Error"; break;
                case ::scaffold::LogLevel::Warn: str = "Warn"; break;
                case ::scaffold::LogLevel::Info: str = "Info"; break;
                case ::scaffold::LogLevel::Debug: str = "Info"; break;
            }

            return format_to(ctx.out(), "{}", str);
        }
    };
}