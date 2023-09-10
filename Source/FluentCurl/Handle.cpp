#include <FluentCurl/Handle.hpp>

namespace FluentCurl
{
Handle::Handle(const Handle& copy):
	_curl_opts_and_params(copy._curl_opts_and_params)
{
}
Handle&
Handle::operator=(const Handle& copy)
{
	if(this != &copy)
		_curl_opts_and_params = copy._curl_opts_and_params;

	return *this;
}
Handle::Handle(Handle&& move) noexcept:
	_curl_opts_and_params(std::move(move._curl_opts_and_params))
{
	move._curl_opts_and_params.clear();
}
Handle&
Handle::operator=(Handle&& move) noexcept
{
	if(this == &move)
		return *this;

	_curl_opts_and_params = std::move(move._curl_opts_and_params);

	move._curl_opts_and_params.clear();

	return *this;
}
void
Handle::throw_on_curl_easy_error(CURLcode result)
{
	if(result != CURLE_OK)
		throw std::runtime_error(curl_easy_strerror(result));
}
Handle&
Handle::reset()
{
	_curl_opts_and_params.clear();

	return *this;
}
void
Handle::configure_curl_handle(CURL* handle) const
{
	for(const auto&[opt, param]: _curl_opts_and_params)
	{
		throw_on_curl_easy_error(
			curl_easy_setopt(handle, opt, param));
	}
}
}