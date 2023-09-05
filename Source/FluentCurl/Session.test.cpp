#include <thread>

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
TEST(Session, default_constructor)
{
	Session session;
}
TEST(Session, should_perform_handle)
{
	Handle handle;

	std::string write_to;
	std::string expected = "<!doctype html>\n"
						   "<html>\n"
						   "<head>\n"
						   "    <title>Example Domain</title>\n"
						   "\n"
						   "    <meta charset=\"utf-8\" />\n"
						   "    <meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\" />\n"
						   "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n"
						   "    <style type=\"text/css\">\n"
						   "    body {\n"
						   "        background-color: #f0f0f2;\n"
						   "        margin: 0;\n"
						   "        padding: 0;\n"
						   "        font-family: -apple-system, system-ui, BlinkMacSystemFont, \"Segoe UI\", \"Open Sans\", \"Helvetica Neue\", Helvetica, Arial, sans-serif;\n"
						   "        \n"
						   "    }\n"
						   "    div {\n"
						   "        width: 600px;\n"
						   "        margin: 5em auto;\n"
						   "        padding: 2em;\n"
						   "        background-color: #fdfdff;\n"
						   "        border-radius: 0.5em;\n"
						   "        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);\n"
						   "    }\n"
						   "    a:link, a:visited {\n"
						   "        color: #38488f;\n"
						   "        text-decoration: none;\n"
						   "    }\n"
						   "    @media (max-width: 700px) {\n"
						   "        div {\n"
						   "            margin: 0 auto;\n"
						   "            width: auto;\n"
						   "        }\n"
						   "    }\n"
						   "    </style>    \n"
						   "</head>\n"
						   "\n"
						   "<body>\n"
						   "<div>\n"
						   "    <h1>Example Domain</h1>\n"
						   "    <p>This domain is for use in illustrative examples in documents. You may use this\n"
						   "    domain in literature without prior coordination or asking for permission.</p>\n"
						   "    <p><a href=\"https://www.iana.org/domains/example\">More information...</a></p>\n"
						   "</div>\n"
						   "</body>\n"
						   "</html>\n";

	handle
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>(write_cb)
		.option<CURLOPT_WRITEDATA>(&write_to);

	Session session;

	session.perform(handle);

	EXPECT_EQ(expected, write_to);
}
TEST(Session, should_perform_handle_thread_safe)
{
	Handle handle_0;
	Handle handle_1;

	std::string write_to_0;
	std::string write_to_1;

	handle_0
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>(write_cb)
		.option<CURLOPT_WRITEDATA>(&write_to_0);

	handle_1
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>(write_cb)
		.option<CURLOPT_WRITEDATA>(&write_to_1);

	Session session;

	std::thread worker_0(&Session::perform, &session, handle_0);
	std::thread worker_1(&Session::perform, &session, handle_1);

	worker_0.join();
	worker_1.join();
}