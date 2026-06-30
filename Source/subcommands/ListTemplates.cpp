#include <SubCommands.h>

#include <Config.h>

namespace scaffold
{
    static void Run()
    {
        std::error_code ec;

        Config cfg = LoadConfig(AppPath);

        fs::path template_dir = fs::absolute(cfg.template_dir);
        if (!fs::is_directory(template_dir, ec) && !ec)
        {
            SCFLD_LOG_ERROR("Error?");
            exit(1);
        }
        else if (ec)
        {
            SCFLD_LOG_ERROR(ec.message());
            exit(1);
        }

        if (fs::is_empty(template_dir, ec) && !ec)
        {
            std::cout << std::format("[info] No templates found at '{0}'\n", template_dir.string());
            return;
        }
        else if (ec)
        {
            SCFLD_LOG_ERROR(ec.message());
            exit(1);
        }

        int count = 0;
        
        std::cout << "Templates:\n";
        for (const auto& entry : fs::directory_iterator(template_dir))
        {
            if (entry.is_directory(ec) && !ec)
            {
                std::string name = entry.path().filename().string();
                
                std::format_to(
                    std::ostream_iterator<char>(std::cout),
                    " - {0}\n",
                    name
                );
                count++;
            }
        }

        SCFLD_LOG_INFO(std::format("Total amount of templates: {}", count));
    }

    void SetupListTemplates(CLI::App& app)
    {
        auto sub = app.add_subcommand("list-templates", "Lists available templates.");

        sub->callback([] { Run(); });
    }
}