#pragma once
#include "post.hpp"
#include "user.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Service {
public:
  virtual ~Service() = default;
};

class PostService {
public:
  void addPost(const User &user, const std::string &title,
               const std::string &body);
  std::vector<Post> &getPosts();
  int getPostsSize() const;
  bool postExists(const int &id);

private:
  std::vector<Post> posts_;
};

class UserService {
public:
  void addUser(const std::string &username);
  int getUsersSize() const;
  void printUsers();
  bool userExists(const std::string &username);
  std::vector<User> &getUsers();

private:
  std::vector<User> users_;
};

class Application {
public:
  Application();
  virtual ~Application() = default;

  std::shared_ptr<UserService> getUserService() const;
  std::shared_ptr<PostService> getPostService() const;

private:
  std::shared_ptr<UserService> userService_;
  std::shared_ptr<PostService> postService_;
};
