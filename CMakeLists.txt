cmake_minimum_required(VERSION 3.26)

project(fluent_curl)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O3 -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak -O3 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -O3 -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -O3 -g")

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