#include <SubCommands.h>

#include <Config.h>
#include <Utils/Log.h>

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <format>

namespace scaffold
{
    std::string project_name;
    std::string template_name;
    std::string output_dir;
    std::vector<std::string> vars;
    bool force = false;

    static std::string SubstituteStr(std::string str, const std::map<std::string, std::string>& vars)
    {
        for (const auto& [key, value] : vars)
        {
            std::string token = "~" + key + "~";
            size_t pos = 0;

            while ((pos = str.find(token, pos)) != std::string::npos)
            {
                str.replace(pos, token.size(), value);
                pos += value.size();
            }
        }
        return str;
    }

    static fs::path SubstitutePath(const fs::path& path, const std::map<std::string, std::string>& vars)
    {
        fs::path result;

        for (const auto& part : path)
        {
            result /= SubstituteStr(part.string(), vars);
        }

        return result;
    }

    static void ProcessFile(const fs::path& src, const fs::path& dst, const std::map<std::string, std::string>& vars)
    {
        std::ifstream in(src, std::ios::binary);
        if (!in)
        {
            std::cout << std::format("[warn] Cannot read '{0}' (skipping)\n", src.string());
            return;
        }

        std::string content(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>()
        );

        std::string processed = SubstituteStr(content, vars);

        std::ofstream out(dst, std::ios::binary | std::ios::trunc);
        if (!out)
        {
            std::cout << std::format("[warn] Cannot write '{0}' (skipping)\n", dst.string());
            return;
        }

        out.write(processed.data(), processed.size());
    }

    static void Run()
    {
        std::error_code ec;
        if (output_dir.empty())
        {
            output_dir = project_name;
        }

        Config cfg = LoadConfig(AppPath);

        SCFLD_LOG_INFO(std::format("Project: {}", project_name));
        SCFLD_LOG_INFO(std::format("Template: {}", template_name));
        SCFLD_LOG_INFO(std::format("Output Directory: {}", output_dir));

        fs::path template_path = fs::absolute(cfg.template_dir) / template_name;
        SCFLD_LOG_INFO(std::format("Template Path: {}", template_path.string()));

        if (!EnsureDirectoryExists(template_path))
        {
            SCFLD_LOG_ERROR("Failed to find template (Make sure you have the right template directory).");
            exit(1);
        }

        fs::path output_path(fs::absolute(output_dir));
        if (EnsureDirectoryExists(output_path))
        {
            if (!force)
            {
                SCFLD_LOG_ERROR("Output directory already exists (use -f/--force flag to overwrite)");
                exit(1);
            }

            if (ConfirmAction(std::format("Do you want to remove files from '{0}'?", output_path.string()), false))
            {
                SCFLD_LOG_INFO("Removing existing output directory...");
                fs::remove_all(output_path, ec);
                EnsureNotError(ec);
            }
        }

        SCFLD_LOG_INFO("Parsing variables...");
        auto start = std::chrono::steady_clock::now();

        std::map<std::string, std::string> variables =
        {
            { "project_name", project_name },
            { "template_name", template_name },
            { "output_dir", output_dir }
        };

        for (const auto& var : cfg.global_vars)
        {
            auto pos = var.find('=');
            if (pos == std::string::npos)
            {
                SCFLD_LOG_WARN(std::format("Invalid variable provided: '{}' (expected KEY=VALUE, skipping).", var));
                continue;
            }
            variables[var.substr(0, pos)] = var.substr(pos + 1);
        }

        auto dur = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start);
        SCFLD_LOG_INFO(std::format("Global variables took {} to parse", dur));

        start = std::chrono::steady_clock::now();
        for (const auto& var : vars)
        {
            auto pos = var.find('=');
            if (pos == std::string::npos)
            {
                SCFLD_LOG_WARN(std::format("Invalid variable provided: '{}' (expected KEY=VALUE, skipping).", var));
                continue;
            }
            variables[var.substr(0, pos)] = var.substr(pos + 1);
        }

        dur = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start);
        SCFLD_LOG_INFO(std::format("Custom variables took {} to parse", dur));

        SCFLD_LOG_INFO("Generating files...");
        start = std::chrono::steady_clock::now();

        for (const auto& entry : fs::recursive_directory_iterator(template_path, ec))
        {
            EnsureNotError(ec);

            fs::path rel = fs::relative(entry.path(), template_path, ec);
            EnsureNotError(ec);

            fs::path new_rel = SubstitutePath(rel, variables);
            fs::path dest = output_path / new_rel;

            if (entry.is_directory())
            {
                fs::create_directories(dest, ec);
                EnsureNotError(ec);
            }
            else if (entry.is_regular_file())
            {
                fs::create_directories(dest.parent_path(), ec);
                EnsureNotError(ec);
                
                ProcessFile(entry.path(), dest, variables);
                SCFLD_LOG_INFO(std::format("Created - {}", new_rel.string()));
            }
        }

        dur = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start);
        SCFLD_LOG_INFO(std::format("Files took {} to generate", dur));
        SCFLD_LOG_INFO("Done");
    }

    void SetupCreate(CLI::App &app)
    {
        auto sub = app.add_subcommand("create", "Creates a project.");

        sub->add_option("name", project_name, "Project name.")->required();
        sub->add_option("template", template_name, "Template name.")->required();
        sub->add_option("-o,--output", output_dir, "Output directory (Project name by default).");
        sub->add_option("-v,--var", vars, "Append/Assign a custom variable (KEY=VALUE).");
        sub->add_flag("-f,--force", force, "Overwrite output directory.");
        sub->callback([] { Run(); });
    }
}