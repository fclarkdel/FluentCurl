#include <fluent_curl/session.hpp>

namespace fluent_curl
{
session::session():
	_multi_handle(curl_multi_init()),
	_event_loop(uv_default_loop()),
	_timer(new uv_timer_t),
	_keep_thread_alive(true)
{
	throw_on_uv_error(
		uv_timer_init(_event_loop, _timer));

	_timer->data = _multi_handle;

	CurlContext curl_context{&_multi_handle, _event_loop};

	throw_on_curl_multi_error(
		curl_multi_setopt(_multi_handle, CURLMOPT_SOCKETFUNCTION, socket_cb));
	throw_on_curl_multi_error(
		curl_multi_setopt(_multi_handle, CURLMOPT_SOCKETDATA, curl_context));
	throw_on_curl_multi_error(
		curl_multi_setopt(_multi_handle, CURLMOPT_TIMERFUNCTION, timer_cb));
	throw_on_curl_multi_error(
		curl_multi_setopt(_multi_handle, CURLMOPT_TIMERDATA, _timer));

	_thread = std::thread(&session::process_handles, this);
}
session::~session()
{
	// We must first kill the thread before locking the queue,
	// otherwise we will be in a deadlock.
	_keep_thread_alive = false;
	_thread.join();

	_queue_lock.lock();

	throw_on_uv_error(
		uv_timer_stop(_timer));

	uv_close((uv_handle_t*)_timer, timer_close_cb);

	while(uv_loop_close(_event_loop) == UV_EBUSY)
	{
		throw_on_uv_error(
			uv_run(_event_loop, UV_RUN_DEFAULT));
	}
	// Replace with a final_check_multi_info
	// that will run until all running curl handles are finished.
	check_multi_info(_multi_handle);

	throw_on_curl_multi_error(
		curl_multi_cleanup(_multi_handle));
}
void
session::add_handle(const handle& handle)
{
	// Block readers while we write,
	// but still allow other writers.
	_queue_lock.lock_shared();

	CURL* curl_handle = curl_easy_init();

	handle.configure_curl_handle(curl_handle);

	_queue.push(curl_handle);

	// Release lock to allow reader to read.
	_queue_lock.unlock_shared();
}
void
session::process_handles()
{
	while(_keep_thread_alive)
	{
		// Block writers while reading.
		std::scoped_lock scoped_lock(_queue_lock);

		while(!_queue.empty())
		{
			CURL* curl_handle = _queue.front();
			_queue.pop();

			throw_on_curl_multi_error(
				curl_multi_add_handle(_multi_handle, curl_handle));
		}
		throw_on_uv_error(
			uv_run(_event_loop, UV_RUN_DEFAULT));

		check_multi_info(_multi_handle);
	}
}
void
session::throw_on_curl_multi_error(CURLMcode result)
{
	if(result != CURLM_OK)
		throw std::runtime_error(curl_multi_strerror(result));
}
int
session::throw_on_uv_error(int result)
{
	if(result >= 0)
		return result;

	std::string error_message;

	error_message += uv_err_name(result);
	error_message += ": ";
	error_message += uv_strerror(result);

	throw std::runtime_error(error_message);
}
void
session::poll_event_cb(
	uv_poll_t* handle,
	int status,
	int events)
{
	throw_on_uv_error(status);

	auto context = (PollContext*)handle->data;

	int flags = 0;

	if(events & UV_READABLE)
		flags |= CURL_CSELECT_IN;
	if(events & UV_WRITABLE)
		flags |= CURL_CSELECT_OUT;

	int running_handles = 0;

	throw_on_curl_multi_error(
		curl_multi_socket_action(context->multi_handle, context->socket, flags, &running_handles));
}
void
session::poll_close_cb(uv_handle_t* handle)
{
	delete(PollContext*)handle->data;
	delete(uv_poll_t*)handle;
}
void
session::timeout_cb(uv_timer_t* handle)
{
	auto multi_handle = (CURLM*)handle->data;

	int running_handles = 0;

	throw_on_curl_multi_error(
		curl_multi_socket_action(multi_handle, CURL_SOCKET_TIMEOUT, 0, &running_handles));
}
void
session::timer_close_cb(uv_handle_t* handle)
{
	delete(uv_timer_t*)handle;
}
int
session::socket_cb(
	CURL* handle,
	curl_socket_t socket,
	int action,
	void* shared_data,
	void* socket_data)
{
	auto curl_context = (CurlContext*)shared_data;

	auto poll_handle = (uv_poll_t*)socket_data;

	switch(action)
	{
	case CURL_POLL_IN:
	case CURL_POLL_OUT:
	case CURL_POLL_INOUT:
		if(poll_handle == nullptr)
		{
			poll_handle = new uv_poll_t;

			throw_on_uv_error(
				uv_poll_init_socket(curl_context->event_loop, poll_handle, socket));

			poll_handle->data = new PollContext{curl_context->multi_handle, socket};

			throw_on_curl_multi_error(
				curl_multi_assign(curl_context->multi_handle, socket, poll_handle));
		}
		{
			int events = 0;

			if(action != CURL_POLL_IN)
				events |= UV_WRITABLE;
			if(action != CURL_POLL_OUT)
				events |= UV_READABLE;

			throw_on_uv_error(
				uv_poll_start(poll_handle, events, poll_event_cb));
		}
		break;
	case CURL_POLL_REMOVE:
		if(poll_handle == nullptr)
			break;

		throw_on_uv_error(
			uv_poll_stop(poll_handle));

		uv_close((uv_handle_t*)poll_handle, poll_close_cb);

		throw_on_curl_multi_error(
			curl_multi_assign(curl_context->multi_handle, socket, nullptr));

		break;
	default:
		throw std::runtime_error("Unknown curl action specified in socket_cb.");
	}
	return 0;
}
int
session::timer_cb(
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
void
session::check_multi_info(CURLM* multi_handle)
{
	int running_handles = 0;

	for(CURLMsg* curl_message = curl_multi_info_read(multi_handle, &running_handles);
		curl_message != nullptr;
		curl_message = curl_multi_info_read(multi_handle, &running_handles))
	{
		CURL* easy_handle = curl_message->easy_handle;

		throw_on_curl_multi_error(
			curl_multi_remove_handle(multi_handle, easy_handle));

		curl_easy_cleanup(easy_handle);
	}
}
}