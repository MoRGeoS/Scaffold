#include <Utils.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include <iostream>
#include <format>

namespace scaffold
{
    Config DefaultConfig(const fs::path& app_path)
    {
        return Config
        {
            .template_dir = (app_path / "templates").string()
        };
    }

    Config LoadConfig(const fs::path& app_path)
    {
        try
        {
            Config config;
            fs::path cfg_path = app_path / ".config";
            if (!fs::exists(cfg_path))
            {
                return DefaultConfig(app_path);
            }
    
            std::ifstream file(cfg_path);
            if (!file.is_open())
            {
                return DefaultConfig(app_path);
            }
    
            auto json = nlohmann::json::parse(file);
            config.template_dir = json["template_dir"];
            config.global_vars = json["global_vars"].get<std::vector<std::string>>();

            return config;
        }
        catch (...)
        {
            return DefaultConfig(app_path);
        }
    }

    bool SaveConfig(const fs::path& app_path, const Config& config)
    {
        try
        {
            fs::create_directories(app_path);
            
            fs::path cfg_path = app_path / ".config";

            nlohmann::json json;
            json["template_dir"] = config.template_dir;
            json["global_vars"] = config.global_vars;

            std::ofstream file(cfg_path);
            if (!file.is_open())
                return false;

            file << json.dump(4);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    void Error(const std::error_code& ec)
    {
        std::cout << std::format("Error: {0}\n", ec.message());
        exit(1);
    }

    void EnsureNotError(const std::error_code& ec)
    {
        if (ec)
        {
            Error(ec);
        }
    }

    bool EnsureDirectoryExists(const fs::path& path)
    {
        std::error_code ec;
        if (fs::exists(path, ec) && !ec && fs::is_directory(path, ec) && !ec)
        {
            return true;
        }
        return false;
    }

    bool EnsureFileExists(const fs::path& path)
    {
        std::error_code ec;
        if (fs::exists(path, ec) && !ec && fs::is_regular_file(path, ec) && !ec)
        {
            return true;
        }
        return false;
    }

    bool ConfirmAction(std::string_view message, bool defValue)
    {
        while (true)
        {
            std::format_to(
                std::ostream_iterator<char>(std::cout),
                "{} {}: ",
                message, defValue ? "[Y/n]" : "[y/N]"
            );

            std::string input;
            std::getline(std::cin, input);

            if (input.empty())
                return defValue;

            if (input == "y" || input == "Y")
                return true;

            if (input == "n" || input == "N")
                return false;
        }
    }
}