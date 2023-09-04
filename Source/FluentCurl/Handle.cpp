#include <FluentCurl/Handle.hpp>

namespace FluentCurl
{
Handle::Handle():
	handle(curl_easy_init())
{
}
Handle::~Handle()
{
	curl_easy_cleanup(handle);
}
Handle::Handle(const Handle& copy):
	handle(curl_easy_duphandle(copy.handle))
{
}
Handle&
Handle::operator=(const Handle& copy)
{
	if(this != &copy)
		handle = curl_easy_duphandle(copy.handle);

	return *this;
}
Handle::Handle(Handle&& move) noexcept:
	handle(move.handle)
{
	move.handle = curl_easy_init();
}
Handle&
Handle::operator=(Handle&& move) noexcept
{
	if(this == &move)
		return *this;

	handle = move.handle;

	move.handle = curl_easy_init();

	return *this;
}
Handle&
Handle::reset()
{
	curl_easy_reset(handle);

	return *this;
}
CURL*
Handle::raw_handle()
{
	return handle;
}
}