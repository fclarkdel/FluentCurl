#ifndef FLUENTCURL_HANDLEOPTIONS_HPP
#define FLUENTCURL_HANDLEOPTIONS_HPP

#define SPECIALIZE_CURL_OPT_PARAM(curl_opt, param_t) \
	template<>                                       \
	struct curl_opt_param<curl_opt>                  \
	{                                                \
		using type = param_t;                        \
	}

#include <any>

#include <curl/curl.h>

namespace FluentCurl
{
template<CURLoption curl_opt>
struct curl_opt_param
{
	static_assert(curl_opt != curl_opt, "A CURLoption must be specialized before it used.");

	using type = std::any;
};
template<CURLoption curl_opt>
using curl_opt_param_t = curl_opt_param<curl_opt>::type;

SPECIALIZE_CURL_OPT_PARAM(CURLOPT_URL, const char*);
SPECIALIZE_CURL_OPT_PARAM(CURLOPT_WRITEFUNCTION, size_t(char*, size_t, size_t, void*));
SPECIALIZE_CURL_OPT_PARAM(CURLOPT_WRITEDATA, void*);
}
#endif