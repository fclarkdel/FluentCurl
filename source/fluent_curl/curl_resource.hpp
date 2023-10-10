#ifndef FLUENTCURL_CURLRESOURCE_HPP
#define FLUENTCURL_CURLRESOURCE_HPP

#include <mutex>

#include <curl/curl.h>

namespace fluent_curl
{
class curl_resource
{
public:
	curl_resource();

	~curl_resource();

private:
	static inline std::mutex _global_lock;
	static inline size_t _curl_resource_count;
};
}
#endif