#ifndef FLUENTCURL_CURLRESOURCE_HPP
#define FLUENTCURL_CURLRESOURCE_HPP

#include <mutex>

#include <curl/curl.h>

namespace FluentCurl
{
class CurlResource
{
public:
	CurlResource();

	~CurlResource();

private:
	static inline std::mutex _global_lock;
	static inline size_t _curl_resource_count;
};
}
#endif