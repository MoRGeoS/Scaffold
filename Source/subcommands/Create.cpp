#include <SubCommands.h>

#include <Config.h>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>
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

        std::cout << std::format("[info] Project: {0}\n", project_name);
        std::cout << std::format("[info] Template: {0}\n", template_name);
        std::cout << std::format("[info] Output Directory: {0}\n", output_dir);

        fs::path template_path = fs::absolute(cfg.template_dir) / template_name;
        std::cout << std::format("[info] Template Path: {0}\n", template_path.string());

        if (!EnsureDirectoryExists(template_path))
        {
            std::cout << std::format("[error] Failed to find template: '{}'", template_name);
            exit(1);
        }

        fs::path output_path(fs::absolute(output_dir));
        if (EnsureDirectoryExists(output_path))
        {
            if (!force)
            {
                std::cout << std::format("[error] Output already exists: '{0}' (use -f to overwrite)\n", output_path.string());
                exit(1);
            }

            if (ConfirmAction(std::format("Do you want to remove files from '{0}'?", output_path.string()), false))
            {
                std::cout << "[info] Removing existing output directory...\n";
                fs::remove_all(output_path, ec);
                EnsureNotError(ec);
            }
        }

        std::cout << "[info] Parsing variables...\n";
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
                std::cout << std::format("[warn] Invalid global variable format: '%s' (expected KEY=VALUE, skipping).\n", var);
                continue;
            }
            variables[var.substr(0, pos)] = var.substr(pos + 1);
        }

        auto dur = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start);
        std::cout << std::format("[info] Global variables took {0} to parse\n", dur);

        start = std::chrono::steady_clock::now();
        for (const auto& var : vars)
        {
            auto pos = var.find('=');
            if (pos == std::string::npos)
            {
                std::cout << std::format("[warn] Invalid custom variable format: '%s' (expected KEY=VALUE, skipping).\n", var);
                continue;
            }
            variables[var.substr(0, pos)] = var.substr(pos + 1);
        }

        dur = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start);
        std::cout << std::format("[info] Custom variables took {0} to parse\n", dur);

        std::cout << "[info] Generating files...\n";
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

                std::cout << std::format("[info] Created - {0}\n", new_rel.string());

                ProcessFile(entry.path(), dest, variables);
            }
        }

        dur = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start);
        std::cout << std::format("[info] Files took {0} to generate\n", dur);

        std::cout << "[info] Done\n";
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