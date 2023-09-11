#ifndef FLUENTCURL_HANDLE_HPP
#define FLUENTCURL_HANDLE_HPP

#include <vector>

#include <curl/curl.h>

#include <fluent_curl/curl_resource.hpp>
#include <fluent_curl/handle_options.hpp>

namespace fluent_curl
{
class handle: public curl_resource
{
public:
	friend class session;

	handle() = default;

	handle(const handle& copy);

	handle&
	operator=(const handle& copy);

	handle(handle&& move) noexcept;

	handle&
	operator=(handle&& move) noexcept;

	template<CURLoption curl_opt>
	handle&
	option(curl_opt_param_t<curl_opt> param)
	{
		_curl_opts_and_params.emplace_back(curl_opt, (void*)std::move(param));

		return *this;
	};

	handle&
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