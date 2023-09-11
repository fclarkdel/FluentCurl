add_library(
    fluent_curl_library
    curl_resource.hpp
    curl_resource.cpp
    handle.hpp
    handle.cpp
    handle_options.hpp
    session.hpp
    session.cpp
)
target_include_directories(
    fluent_curl_library
    PUBLIC
    ../../source
)
include(FetchContent)

FetchContent_Declare(
    libcurl
    URL https://github.com/curl/curl/archive/refs/tags/curl-8_1_2.zip
)
FetchContent_Declare(
    libuv
    URL https://github.com/libuv/libuv/archive/refs/tags/v1.46.0.zip
)
FetchContent_MakeAvailable(
    libcurl
    libuv
)
target_link_libraries(
    fluent_curl_library
    PUBLIC
    libcurl
    uv
)
if(PROJECT_IS_TOP_LEVEL)
    add_executable(
        fluent_curl_tests
        handle.test.cpp
        session.test.cpp
    )
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(
        fluent_curl_tests
        PRIVATE
        fluent_curl_library
        gmock_main
    )
    enable_testing()
endif()