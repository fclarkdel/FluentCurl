cmake_minimum_required(VERSION 3.26)

project(fluent_curl)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(source/fluent_curl)

add_library(
    fluent_curl
    INTERFACE
)
target_link_libraries(
    fluent_curl
    INTERFACE
    fluent_curl_library
)