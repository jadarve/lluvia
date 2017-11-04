
test_macro()

function(add_gtest NAME SRCS)

    set(args_list --gtest_output=xml:test/${NAME}.xml)
    add_executable(${NAME} ${SRCS})
    target_link_libraries(${NAME} vulkan lluvia-core ${GTEST_BOTH_LIBRARIES} pthread)
    GTEST_ADD_TESTS(${NAME} "${args_list}" ${SRCS})

endfunction()


include_directories(include)
add_library(lluvia-core SHARED
    src/Session.cpp
    src/Buffer.cpp
    src/Memory.cpp
    src/Shader.cpp
    src/impl/MemoryFreeSpaceManager.cpp
)

target_link_libraries(lluvia-core vulkan)


# Add test cpp file
# add_executable(runUnitTests test/test_Test.cpp)
# target_link_libraries(runUnitTests gtest_main)
# add_test(NAME runUnitTests COMMAND runUnitTests)

add_gtest(test_VulkanDriver             test/test_VulkanDriver.cpp)
add_gtest(test_MemoryFreeSpaceManager   test/test_MemoryFreeSpaceManager.cpp)
add_gtest(test_SessionCreation          test/test_SessionCreation.cpp)
add_gtest(test_BufferCreation           test/test_BufferCreation.cpp)
