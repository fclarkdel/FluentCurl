#include <fluent_curl/session.hpp>

namespace fluent_curl
{
void
session::perform(const handle& handle)
{
	CURL* easy_handle = curl_easy_init();

	handle.configure_curl_handle(easy_handle);

	std::jthread handle_thread([&]()
							  {
								  curl_easy_perform(easy_handle);

								  curl_easy_cleanup(easy_handle);
							  });
}
}