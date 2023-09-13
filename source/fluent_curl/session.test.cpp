#include <thread>

#include <gtest/gtest.h>
#include <httplib.h>

#include <fluent_curl/session.hpp>

using namespace fluent_curl;

class session_test: public ::testing::Test
{
protected:
	httplib::Server svr;
	std::thread svr_thread;
	std::string expected;

	void
	SetUp() override
	{
		svr.Get("/hi", [](const httplib::Request&, httplib::Response& res)
				{
					res.set_content("Hello World!", "text/plain");
				});
		svr.set_keep_alive_timeout(0);

		svr_thread = std::thread([&]()
								 {
									 svr.listen("0.0.0.0", 8080);
								 });

		expected = "Hello World!";
	}
	void
	TearDown() override
	{
		svr.stop();
		svr_thread.join();
	}
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
	handle handle;

	std::string write_to;

	handle
		.option<CURLOPT_URL>("localhost")
		.option<CURLOPT_PORT>(8080)
		.option<CURLOPT_WRITEFUNCTION>(write_cb)
		.option<CURLOPT_WRITEDATA>(&write_to);
	{
		session session;

		session.perform(handle);
	}
	EXPECT_EQ(expected, write_to);
}