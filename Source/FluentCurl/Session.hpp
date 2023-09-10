#ifndef FLUENTCURL_SESSION_HPP
#define FLUENTCURL_SESSION_HPP

#include <shared_mutex>
#include <queue>
#include <thread>

#include <curl/curl.h>
#include <uv.h>

#include <FluentCurl/Handle.hpp>

namespace FluentCurl
{
class Session: public CurlResource
{
public:
	Session();

	~Session();

	Session(const Session& copy) = delete;

	Session&
	operator=(const Session& copy) = delete;

	Session(Session&& move) = delete;

	Session&
	operator=(Session&& move) = delete;

	void
	add_handle(const Handle& handle);

private:
	CURLM* _multi_handle;
	uv_loop_t* _event_loop;
	uv_timer_t* _timer;

	bool _keep_thread_alive;
	std::thread _thread;

	std::shared_mutex _queue_lock;
	std::queue<CURL*> _queue;

	void
	process_handles();

	static void
	throw_on_curl_multi_error(CURLMcode result);

	static int
	throw_on_uv_error(int result);

	// libuv structs and callbacks.
	struct PollContext
	{
		CURLM* multi_handle;
		curl_socket_t socket;
	};

	static void
	poll_event_cb(
		uv_poll_t* handle,
		int status,
		int events);

	static void
	poll_close_cb(uv_handle_t* handle);

	static void
	timeout_cb(uv_timer_t* handle);

	static void
	timer_close_cb(uv_handle_t* handle);

	// curl structs and callbacks.
	struct CurlContext
	{
		CURLM* multi_handle;
		uv_loop_t* event_loop;
	};

	static int
	socket_cb(
		CURL* handle,
		curl_socket_t socket,
		int action,
		void* shared_data,
		void* socket_data);

	static int
	timer_cb(
		CURLM* handle,
		long timeout_ms,
		void* timer_data);

	static void
	check_multi_info(CURLM* handle);
};
}
#endif