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
    res.set(http::field::server, BOOST_BEAST_DEPRECATION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = str;
    res.prepare_payload();
    return res;
  };
  /*
   * /api/user
   */
  if (req.method() == http::verb::post && req.target() == "/api/user") {
    try {
      json reqBody = json::parse(req.body());
      std::string method = reqBody["method"];
      std::string username = reqBody["username"];
      auto userService = app.getUserService();
      if (method == "add") {
        userService->addUser(username);
        return http_response("ok_request", "beast-1 -- user added");
      }
      if (method == "check" && userService->userExists(username)) {
        return http_response("ok_request", "beast-1 -- user exists");
      } else {
        return http_response("bad_request", "beast-1 -- unknown failure");
      }
    } catch (const std::exception &e) {
      return http_response("bad_request", "beast-1 -- invalid JSON body");
    }
  }
  /*
   * /api/post
   * PostService { <posts_> } addPost, getPosts
   * Post { User, title, body, <comments_> } getComments
   * Todo:
   *  add post
   *  get post (and comments)
   */
  if (req.method() == http::verb::post && req.target() == "/api/post") {
    try {
      json reqBody = json::parse(req.body());
      std::string username = reqBody["username"];
      std::string title = reqBody["title"];
      std::string body = reqBody["body"];
      auto postService = app.getPostService();
      auto userService = app.getUserService();
      auto users_ = userService->getUsers();
      for (auto &user : users_) {
        if (userService->userExists(username)) {
          postService->addPost(user, title, body);
          std::cout << "Posts size: " << postService->getPostsSize()
                    << std::endl;
        } else
          http_response("bad_request", "beast-1 -- user does not exist");
      }
      return http_response("ok_request", "beast-1 -- ok");
    } catch (const std::exception &e) {
      return http_response("bad_request", "beast-1 -- invalid JSON body");
    }
  }
  /*
   *
   */
  if (req.method() == http::verb::post && req.target() == "/api/get/post") {
    try {
      json reqBody = json::parse(req.body());

      if (reqBody.contains("id")) {
        int id = reqBody["id"].get<int>();

        auto postService = app.getPostService();

        if (postService->postExists(id)) {
          if (id > 0 && id <= postService->getPostsSize()) {
            const auto &post = postService->getPosts().at(id - 1);
            std::string username = post.getPostUser().getUsername();
            std::string title = post.getTitle();
            std::string body = post.getBody();

            json responseJson;
            responseJson["author"] = username;
            responseJson["title"] = title;
            responseJson["body"] = body;

            return http_response("ok_request", responseJson.dump());
          } else {
            return http_response("bad_request", "beast-1 -- invalid post ID");
          }
        } else {
          return http_response("bad_request", "beast-1 -- post does not exist");
        }
      } else {
        return http_response("bad_request",
                             "beast-1 -- 'id' is missing in the JSON body");
      }
    } catch (const std::exception &e) {
      return http_response("bad_request", "beast-1 -- invalid JSON body");
    }
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
