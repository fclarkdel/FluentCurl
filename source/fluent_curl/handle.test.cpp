#include <gtest/gtest.h>

#include <fluent_curl/handle.hpp>

using namespace fluent_curl;

TEST(handle_test, default_constructor)
{
	handle original;
}
TEST(handle_test, copy_constructor)
{
	handle original;

	handle copy(original);
}
TEST(handle_test, copy_assignment)
{
	handle original;

	handle copy;

	copy = original;
}
TEST(handle_test, move_constructor)
{
	handle original;

	handle move(std::move(original));
}
TEST(handle_test, move_assignment)
{
	handle original;

	handle move;

	move = std::move(original);
}
TEST(handle_test, should_set_options)
{
	handle handle;

	std::string write_to;

	handle
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>([](char*, size_t, size_t, void*)
									   {
										   return size_t{};
									   })
		.option<CURLOPT_WRITEDATA>(&write_to);
}
TEST(handle_test, should_reset_options)
{
	handle handle;

	std::string write_to;

	handle
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>([](char*, size_t, size_t, void*)
									   {
										   return size_t{};
									   })
		.option<CURLOPT_WRITEDATA>(&write_to);

	handle.reset();
}