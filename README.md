# fluent_curl

## Structure

The layout of this project follows the _Canonical Project Structure_ as specified in document _P1204R0_.

## Definitions

This project inherits definitions from the following documents:

* _RFC 2119_

## Objective

To provide a type-safe, asynchronous, fluent interface for _libcurl_ utilizing its _multi_socket API_ and _libuv_.

## Example

```c++
#include <string>

#include <fluent_curl/session.hpp>

using namespace fluent_curl;

size_t
write_cb(
    char* data,
    size_t member_size,
    size_t member_quantity,
    void* user_data)
{
    size_t total_size = member_size * member_quantity;
    
    auto response = static_cast<std::string*>(user_data);
    
    response->append(data, total_size);
    
    return total_size;
}
int main()
{
    std::string write_to;

    handle handle;

    handle
        .option<CURLOPT_URL>("https://example.com/")
        .option<CURLOPT_WRITEFUNCTION>(write_cb)
        .option<CURLOPT_WRITEDATA>(&write_to);

    session session;
    
    session.add_handle(handle);
    
    return 0;
}