#include <curl/curl.h>
#include <gtest/gtest.h>
#include <uv.h>

void
throw_on_uv_error(int result)
{
	if(result >= 0)
		return;

	std::string error_message;

	error_message += uv_err_name(result);
	error_message += ": ";
	error_message += uv_strerror(result);

	throw std::runtime_error(error_message);
}

struct PollContext
{
	CURLM* multi_handle;
	curl_socket_t socket;
};

void
poll_event_cb(
	uv_poll_t* handle,
	int status,
	int events)
{
	std::cout << "event!\n";

	throw_on_uv_error(status);

	auto context = (PollContext*)handle->data;

	int flags = 0;

	if(events & UV_READABLE)
		flags |= CURL_CSELECT_IN;
	if(events & UV_WRITABLE)
		flags |= CURL_CSELECT_OUT;

	int running_handles = 0;

	curl_multi_socket_action(context->multi_handle, context->socket, flags, &running_handles);
}

void
poll_close_cb(
	uv_handle_t* handle)
{
	delete(PollContext*)handle->data;
	delete(uv_poll_t*)handle;
}

struct TimerContext
{
	CURLM* multi_handle;
};

void
timeout_cb(
	uv_timer_t* handle)
{
	auto context = (TimerContext*)handle->data;

	int running_handles = 0;

	curl_multi_socket_action(context->multi_handle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
}

struct CurlContext
{
	CURLM* multi_handle;
	uv_loop_t* event_loop;
};

struct SocketContext
{
	uv_poll_t poll_handle;
};

int
socket_cb(
	CURL* handle,
	curl_socket_t socket,
	int action,
	void* shared_data,
	void* socket_data)
{
	auto curl_context = (CurlContext*)shared_data;

	auto socket_context = (SocketContext*)socket_data;

	switch(action)
	{
	case CURL_POLL_IN:
	case CURL_POLL_OUT:
	case CURL_POLL_INOUT:
		if(socket_context == nullptr)
		{
			socket_context = new SocketContext;

			throw_on_uv_error(
				uv_poll_init_socket(curl_context->event_loop, &socket_context->poll_handle, socket));

			// If uv_poll_init_socket did not error,
			// then poll_handle will not be null.
			socket_context->poll_handle.data = new PollContext{curl_context->multi_handle, socket};

			curl_multi_assign(curl_context->multi_handle, socket, socket_context);
		}
		{
			int events = 0;

			if(action != CURL_POLL_IN)
				events |= UV_WRITABLE;
			if(action != CURL_POLL_OUT)
				events |= UV_READABLE;

			throw_on_uv_error(
				uv_poll_start(&socket_context->poll_handle, events, poll_event_cb));
		}
		break;
	case CURL_POLL_REMOVE:
		if(socket_context == nullptr)
			break;

		throw_on_uv_error(
			uv_poll_stop(&socket_context->poll_handle));

		uv_close((uv_handle_t*)&socket_context->poll_handle, poll_close_cb);

		curl_multi_assign(curl_context->multi_handle, socket, nullptr);

		break;
	default:
		throw std::runtime_error("Unknown curl action specified in socket_cb.");
	}
	return 0;
}

int
timer_cb(
	CURLM* handle,
	long timeout_ms,
	void* timer_data)
{
	auto timer = (uv_timer_t*)timer_data;

	if(timeout_ms < 0)
	{
		throw_on_uv_error(
			uv_timer_stop(timer));
	}
	else
	{
		throw_on_uv_error(
			uv_timer_start(timer, timeout_cb, timeout_ms, 0));
	}
	return 0;
}

size_t
write_func(
	char* data,
	size_t data_member_size,
	size_t data_member_quantity,
	void* userdata)
{
	size_t total_size = data_member_size * data_member_quantity;

	auto response = static_cast<std::string*>(userdata);

	response->append(data, total_size);

	return total_size;
}

TEST(Test, development)
{
	// uv init
	uv_loop_t* event_loop = uv_default_loop();

	uv_timer_t timer;
	uv_timer_init(event_loop, &timer);

	// curl init
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURLM* multi_handle = curl_multi_init();

	timer.data = new TimerContext{multi_handle};

	CurlContext curl_context{&multi_handle, event_loop};
	curl_multi_setopt(multi_handle, CURLMOPT_SOCKETDATA, curl_context);
	curl_multi_setopt(multi_handle, CURLMOPT_SOCKETFUNCTION, socket_cb);

	curl_multi_setopt(multi_handle, CURLMOPT_TIMERFUNCTION, timer_cb);
	curl_multi_setopt(multi_handle, CURLMOPT_TIMERDATA, &timer);

	// start work
	CURL* easy_handle = curl_easy_init();
	curl_easy_setopt(easy_handle, CURLOPT_URL, "https://example.com/");

	std::string write_to;
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, &write_to);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_func);

	curl_multi_add_handle(multi_handle, easy_handle);

	uv_run(event_loop, UV_RUN_DEFAULT);

	std::cout << write_to;

	auto easy_handle2 = curl_easy_duphandle(easy_handle);
	curl_multi_add_handle(multi_handle, easy_handle2);
	std::string write_to2;
	curl_easy_setopt(easy_handle2, CURLOPT_WRITEDATA, &write_to2);

	uv_run(event_loop, UV_RUN_DEFAULT);

	std::cout << write_to2;

	// curl clean up
	curl_multi_remove_handle(multi_handle, easy_handle);
	curl_easy_cleanup(easy_handle);
	curl_multi_cleanup(multi_handle);
	curl_global_cleanup();

	// uv clean up
	uv_loop_close(event_loop);
}