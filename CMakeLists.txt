cmake_minimum_required(VERSION 3.26)

project(FluentCurl)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Source/FluentCurl)

add_library(
    FluentCurl
    INTERFACE
)
target_link_libraries(
    FluentCurl
    INTERFACE
    FluentCurl_Library
)