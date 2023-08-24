add_library(
    CppProjectTemplate_Library
    HelloWorld.hpp
    HelloWorld.cpp
)
target_include_directories(
    CppProjectTemplate_Library
    PUBLIC
    ../../Source
)
include(FetchContent)

if(PROJECT_IS_TOP_LEVEL)
    add_executable(
        CppProjectTemplate_Tests
        HelloWorld.test.cpp
    )
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(
        CppProjectTemplate_Tests
        PRIVATE
        CppProjectTemplate_Library
        gmock_main
    )
    enable_testing()
endif()