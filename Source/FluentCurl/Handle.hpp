#ifndef FLUENTCURL_HANDLE_HPP
#define FLUENTCURL_HANDLE_HPP

#include <curl/curl.h>

#include <FluentCurl/CurlResource.hpp>
#include <FluentCurl/HandleOptions.hpp>

namespace FluentCurl
{
class Handle: public CurlResource
{
public:
	Handle();

	~Handle();

	Handle(const Handle& copy);

	Handle&
	operator=(const Handle& copy);

	Handle(Handle&& move) noexcept;

	Handle&
	operator=(Handle&& move) noexcept;

	template<CURLoption curl_opt>
	Handle&
	option(curl_opt_param_t<curl_opt> param)
	{
		CURLcode res = curl_easy_setopt(handle, curl_opt, param);

		if(res != CURLE_OK)
			throw std::runtime_error(curl_easy_strerror(res));

		return *this;
	};

	Handle&
	reset();

	CURL*
	raw_handle();

private:
	CURL* handle;
};
}
#endif