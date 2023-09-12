#ifndef FLUENTCURL_SESSION_HPP
#define FLUENTCURL_SESSION_HPP

#include <mutex>
#include <queue>
#include <thread>

#include <curl/curl.h>
#include <uv.h>

#include <fluent_curl/curl/curl_resource.hpp>
#include <fluent_curl/handle.hpp>

namespace fluent_curl
{
class session: public curl_resource
{
public:
	session();

	~session();

	session(const session& copy) = delete;

	session&
	operator=(const session& copy) = delete;

	session(session&& move) = delete;

	session&
	operator=(session&& move) = delete;

	void
	perform(const handle& handle);

private:
};
}
#endif