#include "../../include/server/handler.hpp"
void fail(beast::error_code ec, char const *what) {
  std::cerr << what << ": " << ec.message() << "\n";
}
template <class Body, class Allocator>
http::message_generator
handle_request(Application &app,
               http::request<Body, http::basic_fields<Allocator>> &&req) {
  std::map<std::string, http::status> statusMap = {
      {"bad", http::status::bad_request},
      {"not_found", http::status::not_found},
      {"server_error", http::status::internal_server_error},
      {"ok_request", http::status::ok}};
  auto const http_response = [&req, &statusMap](std::string which,
                                                boost::beast::string_view str) {
    auto status = statusMap[which];
    http::response<http::string_body> res{status, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = str;
    res.prepare_payload();
    return res;
  };
  if (req.method() == http::verb::post && req.target() == "/test") {
    try {
      std::cout << req.body() << std::endl;
      json reqBody = json::parse(req.body());
      std::string username = reqBody["username"];
      // auto clientService = app.getClientService();
      auto userService = app.getUserService();
      // std::string response = clientService->makeRequest(
      //     "localhost", "8081", "/test", "test", http::verb::post);
      std::cout << "test1" << std::endl;
      userService->addUser(username);
      std::cout << "test2" << std::endl;
      int userSize = userService->getUsersSize();
      std::cout << "userService->getUsersSize(): " << userSize << std::endl;
      userService->printUsers();
      return http_response("ok_request", "ok--beast-1");

    } catch (const std::exception &e) {
      return http_response("bad_request", "beast-1 -- invalid JSON body");
    }
  }
  if (req.method() == http::verb::get && req.target() == "/test-beast2") {
    auto clientService = app.getClientService();
    std::string response = clientService->makeRequest("localhost", "8081", "/");
    return http_response("ok_request", response);
  }

  if (req.method() != http::verb::get && req.method() != http::verb::head)
    return http_response("bad", "Unknown HTTP-method");
  if (req.target().empty() || req.target()[0] != '/' ||
      req.target().find("..") != boost::beast::string_view::npos)
    return http_response("bad", "Illegal request-target");
  return http_response("bad", "unknown target");
}
template http::message_generator handle_request(
    Application &app,
    http::request<http::string_body, http::basic_fields<std::allocator<char>>>
        &&req);
