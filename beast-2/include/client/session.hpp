#pragma once

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
  const std::string &getResponse() const;

  explicit session(net::io_context &ioc);
  void fail(beast::error_code ec, char const *what);
  void run(std::string host, std::string port, std::string target);

  void run(std::string host, std::string port, std::string target,
           std::string body, http::verb method = http::verb::get);

private:
  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

  void on_connect(beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type);

  void on_write(beast::error_code ec, std::size_t bytes_transferred);

  void on_read(beast::error_code ec, std::size_t bytes_transferred);

private:
  tcp::resolver resolver_;
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  http::request<http::empty_body> req_;
  http::response<http::string_body> res_;
};
