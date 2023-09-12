#include <gtest/gtest.h>
#include <httplib.h>

#include <fluent_curl/curl/curl_multi_socket_handle.hpp>

using namespace fluent_curl;

size_t
write_cb(
	char* data,
	size_t member_size,
	size_t member_quantity,
	void* userdata)
{
	size_t total_size = member_size * member_quantity;

	auto response = static_cast<std::string*>(userdata);

	response->append(data, total_size);

	return total_size;
}
TEST(curl_multi_socket_handle, perform_should_perform_handle)
{
	httplib::Server svr;

	svr.Get("/hi", [](const httplib::Request&, httplib::Response& res)
			{
				res.set_content("Hello World!", "text/plain");
			});
	svr.set_keep_alive_timeout(0);

	std::thread server_thread([&]()
							  {
								  svr.listen("0.0.0.0", 8080);
							  });

	std::string expected = "Hello World!";

	std::size_t count = 200;

	std::vector<CURL*> easy_handles;
	easy_handles.reserve(count);

	std::vector<std::string> results;
	results.reserve(count);

	curl_multi_socket_handle socket_handle;

	for(std::size_t index = 0; index < count; ++index)
	{
		easy_handles.push_back(curl_easy_init());

		curl_easy_setopt(easy_handles[index], CURLOPT_URL, "localhost/hi");
		curl_easy_setopt(easy_handles[index], CURLOPT_PORT, 8080);
		curl_easy_setopt(easy_handles[index], CURLOPT_WRITEFUNCTION, write_cb);
		curl_easy_setopt(easy_handles[index], CURLOPT_WRITEDATA, &results[index]);

		socket_handle.add_handle(easy_handles[index]);
	}
	socket_handle.perform();

	svr.stop();

	server_thread.join();

	for(const auto& result: results)
		EXPECT_EQ(result, expected);
}