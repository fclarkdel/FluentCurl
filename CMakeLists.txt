cmake_minimum_required(VERSION 3.26)

project(CppProjectTemplate)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Source/CppProjectTemplate)

add_library(
    CppProjectTemplate
    INTERFACE
)
target_link_libraries(
    CppProjectTemplate
    INTERFACE
    CppProjectTemplate_Library
)