#ifndef FLUENTCURL_HANDLE_HPP
#define FLUENTCURL_HANDLE_HPP

#include <vector>

#include <curl/curl.h>

#include <FluentCurl/CurlResource.hpp>
#include <FluentCurl/HandleOptions.hpp>

namespace FluentCurl
{
class Handle: public CurlResource
{
public:
	friend class Session;

	Handle() = default;

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
		_curl_opts_and_params.emplace_back(curl_opt, (void*)std::move(param));

		return *this;
	};

	Handle&
	reset();

private:
	std::vector<curl_opt_and_param> _curl_opts_and_params;

	static void
	throw_on_curl_easy_error(CURLcode result);

	void
	configure_curl_handle(CURL* handle) const;
};
}
#endif