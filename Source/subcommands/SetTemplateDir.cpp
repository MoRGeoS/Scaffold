#include <SubCommands.h>

#include <Config.h>
#include <string>
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
            SCFLD_LOG_ERROR(std::format("The path provided is not a directory: '{}'", directory.string()));
            exit(1);
        }
        else if (ec)
        {
            SCFLD_LOG_ERROR(ec.message());
            exit(1);
        }

        cfg.template_dir = directory.string();
        SCFLD_LOG_INFO(std::format("Template Directory: '{}'", directory.string()));

        SCFLD_LOG_INFO("Saving config...");
        if (!SaveConfig(AppPath, cfg))
        {
            SCFLD_LOG_ERROR("Failed to save config!");
            exit(1);
        }

        SCFLD_LOG_INFO("Done");
    }

    void SetupSetTemplateDir(CLI::App& app)
    {
        auto sub = app.add_subcommand("set-template-dir", "Sets template directory.");

        sub->add_option("dir", dir, "New template directory.")->required();\
        sub->callback([] { Run(); });
    }
}