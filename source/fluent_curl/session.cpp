#include <fluent_curl/session.hpp>

namespace fluent_curl
{
session::session()
{
	_keep_thread_alive = true;

	_thread = std::thread(&session::thread_loop, this);
}
session::~session()
{
	// Tear down the queue processing thread.
	_keep_thread_alive = false;

	_thread.join();

	// Clean up the queue if any messages got in
	// while we were cleaning up the processing thread.
	std::scoped_lock scoped_lock(_queue_lock);

	if(!_queue.empty())
		process_queue();
}
void
session::perform(const handle& handle)
{
	CURL* easy_handle = curl_easy_init();

	handle.configure_curl_handle(easy_handle);

	// Lock the queue while we are writing to it.
	std::scoped_lock scoped_lock(_queue_lock);

	_queue.push(easy_handle);

	_queue_condition_variable.notify_one();
}
void
session::process_queue()
{
	while(!_queue.empty())
	{
		_curl_multi_socket_handle.add_handle(_queue.front());
		_queue.pop();
	}
	_curl_multi_socket_handle.perform();
}
void
session::thread_loop()
{
	while(_keep_thread_alive)
	{
		// Acquire the queue lock such that it
		// is not written to while we are processing it.
		std::unique_lock unique_lock(_queue_lock);

		// Wait on the queue to not be empty.
		_queue_condition_variable.wait(unique_lock, [&]()
									   {
										   return !_keep_thread_alive || !_queue.empty();
									   });

		process_queue();
	}
}
}