
#include "lluvia/core.h"

#include "CLI11/CLI11.hpp"

#include <iostream>
#include <cstddef>
#include <string>


int main(int argc, char const* argv[])
{
    CLI::App app{"App description"};

    auto filename = std::string {};

    app.add_option("filename", filename, "Lua script file name")->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    auto interpreter = ll::Interpreter {};
    interpreter.runFile(filename);

    return EXIT_SUCCESS;
}
