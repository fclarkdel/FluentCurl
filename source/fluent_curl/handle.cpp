#include <fluent_curl/handle.hpp>

namespace fluent_curl
{
handle::handle(const handle& copy):
	_curl_opts_and_params(copy._curl_opts_and_params)
{
}
handle&
handle::operator=(const handle& copy)
{
	if(this != &copy)
		_curl_opts_and_params = copy._curl_opts_and_params;

	return *this;
}
handle::handle(handle&& move) noexcept:
	_curl_opts_and_params(std::move(move._curl_opts_and_params))
{
	move._curl_opts_and_params.clear();
}
handle&
handle::operator=(handle&& move) noexcept
{
	if(this == &move)
		return *this;

	_curl_opts_and_params = std::move(move._curl_opts_and_params);

	move._curl_opts_and_params.clear();

	return *this;
}
void
handle::throw_on_curl_easy_error(CURLcode result)
{
	if(result != CURLE_OK)
		throw std::runtime_error(curl_easy_strerror(result));
}
handle&
handle::reset()
{
	_curl_opts_and_params.clear();

	return *this;
}
void
handle::configure_curl_handle(CURL* handle) const
{
	for(const auto& [opt, param]: _curl_opts_and_params)
	{
		throw_on_curl_easy_error(
			curl_easy_setopt(handle, opt, param));
	}
}
handle&
handle::set_transfer_complete_cb(transfer_complete_cb trasfer_complete_cb)
{
	_transfer_complete_cb = trasfer_complete_cb;

	return *this;
}
handle::transfer_complete_cb*
handle::get_transfer_complete_cb() const
{
	return _transfer_complete_cb;
}
handle&
handle::set_transfer_complete_data(void* data)
{
	_transfer_complete_data = data;

	return *this;
}
void*
handle::get_transfer_complete_data() const
{
	return _transfer_complete_data;
}
}