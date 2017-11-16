
include_directories(../../core/include)

add_executable(ll-info
    main.cpp)

target_link_libraries(ll-info lluvia-core vulkan stdc++)