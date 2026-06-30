#include <CLI/CLI.hpp>

#include <Config.h>
#include <SubCommands.h>

int main(int argc, char** argv)
{
    AppPath = std::getenv("APPDATA");
    AppPath = AppPath / "Scaffold";

    CLI::App app;
    scaffold::SetupCommands(app);

    CLI11_PARSE(app, argc, argv);
    return 0;
}