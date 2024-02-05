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
  if (req.method() == http::verb::post && req.target() == "/test-2") {
    try {
      json reqBody = json::parse(req.body());
      std::string username = reqBody["username"];

      // Create a new JSON response
      json responseJson;
      responseJson["username"] = "response_value"; // Modify as needed
      std::string responseBodyStr = responseJson.dump();

      // Use httplib to make the request
      httplib::Client cli("localhost", 8080);
      auto res = cli.Post("/test", responseBodyStr, "application/json");

      if (res && res->status == 200) {
        return http_response("ok_request", res->body);
      } else {
        return http_response("server_error", "beast-2 -- HTTP request failed");
      }
    } catch (const json::exception &e) {
      return http_response("bad_request", "beast-2 -- invalid JSON format");
    }
  }
  if (req.method() == http::verb::post && req.target() == "/test") {
    try {
      auto clientService = app.getClientService();
      json reqBody = json::parse(req.body());
      std::string username = reqBody["username"];

      // Your existing code for extracting username

      // Create a new JSON response
      json responseJson;
      responseJson["username"] = "response_value"; // Modify as needed
      std::string responseBodyStr = responseJson.dump();

      // Make the request using the extracted username and new response JSON
      std::string response = clientService->makeRequest(
          "localhost", "8080", "/test", responseBodyStr, http::verb::post);

      return http_response("ok_request", response);
    } catch (const json::exception &e) {
      return http_response("bad_request", "beast-2 -- invalid JSON format");
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
  return http_response("bad", "beast-2 unknown target");
}
template http::message_generator handle_request(
    Application &app,
    http::request<http::string_body, http::basic_fields<std::allocator<char>>>
        &&req);
