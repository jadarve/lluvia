# include_directories(../../core/include)

add_executable(ll-lua
    main.cpp
)

target_link_libraries(ll-lua cli11 lluvia-core vulkan stdc++ m)
