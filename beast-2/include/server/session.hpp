#pragma once

#include "../application.hpp"
#include "handler.hpp"
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class session_ : public std::enable_shared_from_this<session_> {
public:
  session_(tcp::socket &&socket, Application &app);

  void run();

private:
  void do_read();

  void on_read(beast::error_code ec, std::size_t bytes_transferred);

  void send_response(http::message_generator &&msg);

  void on_write(bool keep_alive, beast::error_code ec,
                std::size_t bytes_transferred);

  void do_close();

private:
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> req_;
  Application &app_;
};
