#include "../include/application.hpp"

Application::Application()
    : userService_(std::make_shared<UserService>()),
      postService_(std::make_shared<PostService>()) {}

std::shared_ptr<UserService> Application::getUserService() const {
  return userService_;
}

std::shared_ptr<PostService> Application::getPostService() const {
  return postService_;
}

void UserService::addUser(const std::string &username) {
  users_.emplace_back(username);
}

int UserService::getUsersSize() const { return users_.size(); }

void UserService::printUsers() {
  for (auto &user : users_) {
    std::cout << user.getUsername() << std::endl;
  }
}

std::vector<User> &UserService::getUsers() { return users_; }

bool UserService::userExists(const std::string &username) {
  for (auto &user : users_) {
    if (user.getUsername() == username)
      return true;
  }
  return false;
}

void PostService::addPost(const User &user, const std::string &title,
                          const std::string &body) {
  posts_.emplace_back(user, title, body);
}

bool PostService::postExists(const int &id) {
  return id > 0 && id <= posts_.size();
}

int PostService::getPostsSize() const { return posts_.size(); }

std::vector<Post> &PostService::getPosts() { return posts_; }
