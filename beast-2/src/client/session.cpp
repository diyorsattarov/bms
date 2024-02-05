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
                  std::string body, http::verb method,
                  std::function<void(std::string)> callback) {
  req_.method(method);
  req_.target(target);
  req_.set(http::field::host, host);
  req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req_.set(http::field::content_type, "application/json");
  req_.set(http::field::body, body);
  req_.prepare_payload();
  std::cout << "[beast-2] run body" << body << std::endl;
  setCallback(callback);
  resolver_.async_resolve(
      host, port,
      beast::bind_front_handler(&session::on_resolve, shared_from_this()));
}

void session::on_resolve(beast::error_code ec,
                         tcp::resolver::results_type results) {
  if (ec)
    return fail(ec, "resolve");

  // Establish a connection
  stream_.expires_after(std::chrono::seconds(30));
  stream_.async_connect(results, beast::bind_front_handler(&session::on_connect,
                                                           shared_from_this()));
}

void session::on_connect(beast::error_code ec,
                         tcp::resolver::results_type::endpoint_type) {
  if (ec)
    return fail(ec, "connect");

  // After connection is established, initiate the read operation
  stream_.expires_after(std::chrono::seconds(30));
  http::async_write(
      stream_, req_,
      beast::bind_front_handler(&session::on_write, shared_from_this()));
}

void session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec)
    return fail(ec, "write");

  // After writing, initiate the read operation to receive the server's response
  http::async_read(
      stream_, buffer_, res_,
      beast::bind_front_handler(&session::on_read, shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec)
    return fail(ec, "read");

  // Now, you can access the server's response via res_.body()
  if (callback_) {
    callback_(res_.body()); // Use callback_ instead of callback
  }

  // Shutdown the socket if necessary
  stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
  if (ec && ec != beast::errc::not_connected)
    return fail(ec, "shutdown");
}
