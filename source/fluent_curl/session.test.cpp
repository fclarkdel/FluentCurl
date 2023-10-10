#include <thread>

#include <gtest/gtest.h>
#include <httplib.h>

#include <fluent_curl/session.hpp>

using namespace fluent_curl;

class session_test: public ::testing::Test
{
};
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
TEST_F(session_test, default_constructor)
{
	session session;
}
TEST_F(session_test, perform_should_perform_handle)
{
	httplib::Server svr;
	std::jthread svr_thread;
	std::string expected;

	svr.Get("/hi", [](const httplib::Request&, httplib::Response& res)
			{
				res.set_content("Hello World!", "text/plain");
			});
	svr.set_keep_alive_timeout(0);

	svr_thread = std::jthread([&]()
							 {
								 svr.listen("0.0.0.0", 8080);
							 });

	expected = "Hello World!";

	std::size_t count = 1000;

	std::vector<handle> handles;
	handles.reserve(count);

	std::vector<std::string> results(count);
	{
		session session;

		for(std::size_t index = 0; index < count; ++index)
		{
			handles.emplace_back();

			handles[index]
				.option<CURLOPT_URL>("localhost/hi")
				.option<CURLOPT_PORT>(8080)
				.option<CURLOPT_WRITEFUNCTION>(write_cb)
				.option<CURLOPT_WRITEDATA>(&results[index]);
		}
		for(const auto& handle: handles)
			session.perform(handle);
	}
	for(const auto& result: results)
		EXPECT_EQ(result, expected);

	svr.stop();
}