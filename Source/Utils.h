#pragma once

#include <Config.h>
#include <filesystem>

namespace fs = std::filesystem;

inline fs::path AppPath;

namespace scaffold
{
    Config LoadConfig(const fs::path& app_path);
    bool SaveConfig(const fs::path& app_path, const Config& config);

    void Error(const std::error_code& ec);
    void EnsureNotError(const std::error_code& ec);
    bool EnsureDirectoryExists(const fs::path& path);
    bool EnsureFileExists(const fs::path& path);

    bool ConfirmAction(std::string_view message, bool defValue = true);
}