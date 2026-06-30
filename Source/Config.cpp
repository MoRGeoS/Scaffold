#include <Config.h>

#include <fstream>
#include <nlohmann/json.hpp>

namespace scaffold
{
    bool Config::Save(const fs::path& path) const
    {
        try
        {
            fs::create_directories(path.parent_path());

            nlohmann::json json;
            json["template_dir"] = this->template_dir;
            json["global_vars"] = this->global_vars;

            std::ofstream f(path);
            if (!f.is_open()) return false;
            f << json.dump(4);
        }
        catch (...)
        {
            return false;
        }
        return true;
    }

    bool Config::Load(const fs::path& path)
    {
        try 
        {
            std::ifstream file(path);
            if (!file.is_open()) return false;

            auto json = nlohmann::json::parse(file);
            
            this->template_dir = json["template_dir"];
            this->global_vars = json["global_vars"].get<std::vector<std::string>>();
        }
        catch (...)
        {
            return false;
        }
        return true;
    }

    void Config::Setup(const fs::path& app_path)
    {
        this->template_dir = (app_path / "templates").string();
    }
}