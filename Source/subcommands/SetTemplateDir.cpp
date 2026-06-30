#include <SubCommands.h>

#include <Config.h>
#include <string>
#include <iostream>
#include <format>

namespace scaffold
{
    std::string dir;

    static void Run()
    {
        std::error_code ec;

        Config cfg = LoadConfig(AppPath);

        fs::path directory = fs::absolute(dir);
        if (!fs::is_directory(directory, ec) && !ec)
        {
            std::cout << std::format("[error] Provided path is not a directory: '{0}'\n", directory.string());
            exit(1);
        }
        else if (ec)
        {
            std::cerr << std::format("[error] {0}\n", ec.message());
            exit(1);
        }

        cfg.template_dir = directory.string();
        std::cout << std::format("[info] Template Directory: '{0}'\n", directory.string());

        std::cout << "[info] Saving config...\n";
        if (!SaveConfig(AppPath, cfg))
        {
            std::cout << std::format("[error] Failed to save config! (AppPath: {0})\n", AppPath.string());
            exit(1);
        }

        std::cout << "[info] Done\n";
    }

    void SetupSetTemplateDir(CLI::App& app)
    {
        auto sub = app.add_subcommand("set-template-dir", "Sets template directory.");

        sub->add_option("dir", dir, "New template directory.")->required();\
        sub->callback([] { Run(); });
    }
}