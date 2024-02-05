#pragma once

#include "../application.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <httplib.h>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
namespace http = boost::beast::http;
using json = nlohmann::json;
void fail(beast::error_code ec, char const *what);
template <class Body, class Allocator>
http::message_generator
handle_request(Application &app,
               http::request<Body, http::basic_fields<Allocator>> &&req);
