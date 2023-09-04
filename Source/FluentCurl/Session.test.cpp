#include <gtest/gtest.h>

#include <FluentCurl/Session.hpp>

using namespace FluentCurl;

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

TEST(Session, should_perform_handle_when_added)
{
	Handle handle;

	std::string write_to;

	handle
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>(write_cb)
		.option<CURLOPT_WRITEDATA>(&write_to);

	Session session;

	session.add_handle(handle);
}