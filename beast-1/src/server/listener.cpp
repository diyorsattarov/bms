#include "../../include/server/listener.hpp"
#include "../../include/server/session.hpp" // Include session header if needed

listener::listener(net::io_context &ioc, tcp::endpoint endpoint,
                   Application &app)
    : ioc_(ioc), acceptor_(net::make_strand(ioc)), app_(app) {
  beast::error_code ec;
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }
  acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }
  acceptor_.bind(endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }
  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }
}

void listener::run() { do_accept(); }

void listener::do_accept() {
  acceptor_.async_accept(
      net::make_strand(ioc_),
      beast::bind_front_handler(&listener::on_accept, shared_from_this()));
}

void listener::on_accept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    fail(ec, "accept");
    return;
  } else {
    std::make_shared<session_>(std::move(socket), app_)->run();
  }
  do_accept();
}
