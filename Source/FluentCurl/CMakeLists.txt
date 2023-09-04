add_library(
    FluentCurl_Library
    CurlResource.hpp
    CurlResource.cpp
    Handle.hpp
    Handle.cpp
    HandleOptions.hpp
    Session.hpp
    Session.cpp

    HelloWorld.hpp
    HelloWorld.cpp
)
target_include_directories(
    FluentCurl_Library
    PUBLIC
    ../../Source
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
    FluentCurl_Library
    PUBLIC
    libcurl
    uv
)
if(PROJECT_IS_TOP_LEVEL)
    add_executable(
        FluentCurl_Tests
        Handle.test.cpp
        Session.test.cpp

        HelloWorld.test.cpp
        Test.test.cpp
    )
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(
        FluentCurl_Tests
        PRIVATE
        FluentCurl_Library
        gmock_main
    )
    enable_testing()
endif()