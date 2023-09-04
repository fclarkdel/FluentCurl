#ifndef FLUENTCURL_SESSION_HPP
#define FLUENTCURL_SESSION_HPP

#include <FluentCurl/Handle.hpp>

namespace FluentCurl
{
class Session: public CurlResource
{
public:
	Session();

	~Session();

	Session(const Session& copy);

	Session&
	operator=(const Session& copy);

	Session(Session&& move) noexcept;

	Session&
	operator=(Session&& move) noexcept;

	void
	add_handle(const Handle& handle);

private:
};
}
#endif