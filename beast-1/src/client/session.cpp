#include "../../include/client/session.hpp"

void session::fail(beast::error_code ec, char const *what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

const std::string &session::getResponse() const { return res_.body(); }

session::session(net::io_context &ioc)
    : resolver_(net::make_strand(ioc)), stream_(net::make_strand(ioc)) {}

void session::run(std::string host, std::string port, std::string target) {
  req_.method(http::verb::get);
  req_.target(target);
  req_.set(http::field::host, host);
  req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  resolver_.async_resolve(
      host, port,
      beast::bind_front_handler(&session::on_resolve, shared_from_this()));
}

void session::run(std::string host, std::string port, std::string target,
                  std::string body, http::verb method) {
  req_.method(method);
  req_.target(target);
  req_.set(http::field::host, host);
  req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req_.set(http::field::content_type, "text/html");
  req_.set(http::field::body, body);
  req_.prepare_payload();

  resolver_.async_resolve(
      host, port,
      beast::bind_front_handler(&session::on_resolve, shared_from_this()));
}

void session::on_resolve(beast::error_code ec,
                         tcp::resolver::results_type results) {
  if (ec)
    return fail(ec, "resolve");
  stream_.expires_after(std::chrono::seconds(30));
  stream_.async_connect(results, beast::bind_front_handler(&session::on_connect,
                                                           shared_from_this()));
}

void session::on_connect(beast::error_code ec,
                         tcp::resolver::results_type::endpoint_type) {
  if (ec)
    return fail(ec, "connect");
  stream_.expires_after(std::chrono::seconds(30));
  http::async_write(
      stream_, req_,
      beast::bind_front_handler(&session::on_write, shared_from_this()));
}

void session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec)
    return fail(ec, "write");
  http::async_read(
      stream_, buffer_, res_,
      beast::bind_front_handler(&session::on_read, shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec)
    return fail(ec, "beast-1 read");
  stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
  if (ec && ec != beast::errc::not_connected)
    return fail(ec, "shutdown");
}
