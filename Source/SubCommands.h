#pragma once

#include <Utils.h>
#include <CLI/App.hpp>

namespace scaffold
{
    void SetupCreate(CLI::App& app);
    void SetupSetTemplateDir(CLI::App& app);
    void SetupListTemplates(CLI::App& app);
    void SetupSetGlobalVars(CLI::App& app);
    void SetupListGlobalVars(CLI::App& app);

    inline void SetupCommands(CLI::App& app)
    {
        SetupCreate(app);
        SetupSetTemplateDir(app);
        SetupListTemplates(app);
        SetupSetGlobalVars(app);
        SetupListGlobalVars(app);
    }
}