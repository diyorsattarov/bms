#include "../../include/server/session.hpp"
#include "../../include/server/listener.hpp"
session_::session_(tcp::socket &&socket, Application &app)
    : stream_(std::move(socket)), app_(app) {}

void session_::run() {
  net::dispatch(
      stream_.get_executor(),
      beast::bind_front_handler(&session_::do_read, shared_from_this()));
}

void session_::do_read() {
  req_ = {};
  stream_.expires_after(std::chrono::seconds(30));
  http::async_read(
      stream_, buffer_, req_,
      beast::bind_front_handler(&session_::on_read, shared_from_this()));
}

void session_::on_read(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec == http::error::end_of_stream)
    return do_close();
  if (ec)
    return fail(ec, "[beast-1] read");
  send_response(handle_request(app_, std::move(req_)));
}

void session_::send_response(http::message_generator &&msg) {
  bool keep_alive = msg.keep_alive();
  beast::async_write(stream_, std::move(msg),
                     beast::bind_front_handler(&session_::on_write,
                                               shared_from_this(), keep_alive));
}

void session_::on_write(bool keep_alive, beast::error_code ec,
                        std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec)
    return fail(ec, "write");
  if (!keep_alive) {
    return do_close();
  }
  do_read();
}

void session_::do_close() {
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}
