
#include "lluvia/core.h"

#include "CLI11/CLI11.hpp"

#include <iostream>
#include <cstddef>
#include <string>


int main(int argc, char const* argv[])
{
    CLI::App app{"Image pyramid with lua nodes"};

    auto filename = std::string {};

    app.add_option("filename", filename, "image file name")->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    std::cout << "filename: " << filename << std::endl;

    return EXIT_SUCCESS;
}
