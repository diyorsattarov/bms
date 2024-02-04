#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <map>
#include <string>

#include "../application.hpp"
#include <nlohmann/json.hpp>
namespace http = boost::beast::http;
using json = nlohmann::json;
void fail(beast::error_code ec, char const *what);
template <class Body, class Allocator>
http::message_generator
handle_request(Application &app,
               http::request<Body, http::basic_fields<Allocator>> &&req);
