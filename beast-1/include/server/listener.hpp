#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>

#include "../application.hpp"

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace beast = boost::beast;
class listener : public std::enable_shared_from_this<listener> {
public:
  listener(net::io_context &ioc, tcp::endpoint endpoint, Application &app);

  void run();

private:
  void do_accept();

  void on_accept(beast::error_code ec, tcp::socket socket);

private:
  net::io_context &ioc_;
  tcp::acceptor acceptor_;
  Application &app_;
};
