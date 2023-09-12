#ifndef FLUENT_CURL_CURL_MULTI_SOCKET_HANDLE_HPP
#define FLUENT_CURL_CURL_MULTI_SOCKET_HANDLE_HPP

#include <curl/curl.h>
#include <uv.h>

#include <fluent_curl/curl/curl_resource.hpp>

namespace fluent_curl
{
class curl_multi_socket_handle: public curl_resource
{
public:
	curl_multi_socket_handle();

	~curl_multi_socket_handle();

	curl_multi_socket_handle(const curl_multi_socket_handle& copy) = delete;

	curl_multi_socket_handle&
	operator=(const curl_multi_socket_handle& copy) = delete;

	curl_multi_socket_handle(curl_multi_socket_handle&& move) = delete;

	curl_multi_socket_handle&
	operator=(curl_multi_socket_handle&& move) = delete;

	void
	add_handle(CURL* handle);

	void
	perform();

private:
	CURLM* _multi_handle;

	uv_loop_t* _event_loop;
	uv_timer_t* _timer;

	static void
	throw_on_curl_multi_error(CURLMcode result);

	static int
	throw_on_uv_error(int result);

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
	timeout_cb(uv_timer_t* handle);

	static void
	poll_close_cb(uv_handle_t* handle);

	static void
	timer_close_cb(uv_handle_t* handle);
};
}
#endif