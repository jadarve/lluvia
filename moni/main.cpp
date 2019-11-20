#include <iostream>

// TODO
// #include "CLI11/CLI11.hpp"

#include <lua.hpp>

// lua libraries are included and linked as expected
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <cassert>

int main()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

    int x = 0;
    lua.set_function("beep", [&x] { ++x; });
    lua.script("beep()");
    lua.script("print('moni moni')");
    assert(x == 1);
}
