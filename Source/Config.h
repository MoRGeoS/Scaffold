#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

namespace scaffold
{
    struct Config
    {
        std::string template_dir;
        std::vector<std::string> global_vars;

        bool Save(const fs::path& path) const;
        bool Load(const fs::path& path);

        void Setup(const fs::path& settings_path);
    };

    inline Config g_Config;
}