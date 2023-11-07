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

								   if(handle.get_transfer_complete_cb() != nullptr)
									   handle.get_transfer_complete_cb()(easy_handle, handle.get_transfer_complete_data());

								   curl_easy_cleanup(easy_handle);
							   });
}
}