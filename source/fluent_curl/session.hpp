#ifndef FLUENTCURL_SESSION_HPP
#define FLUENTCURL_SESSION_HPP

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

#include <curl/curl.h>
#include <uv.h>

#include <fluent_curl/curl_resources/curl_multi_socket_handle.hpp>
#include <fluent_curl/handle.hpp>

namespace fluent_curl
{
class session
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
	std::queue<CURL*> _queue;
	std::mutex _queue_lock;
	std::thread _thread;
	std::atomic_bool _keep_thread_alive;
	curl_multi_socket_handle _curl_multi_socket_handle;

	void
	process_queue();

	void
	thread_loop();
};
}
#endif