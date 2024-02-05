#pragma once
#include "post.hpp"
#include "user.hpp"
#include <string>
#include <vector>
class Comment {
public:
  Comment(const User &user, const std::string &body);

private:
  User user_;
  std::string body_;
};
class Post {
public:
  Post(const User &user, const std::string &title, const std::string &body);
  std::vector<Comment> &getComments();
  void addComment(const User &user, const std::string &body);
  const User &getPostUser() const;
  const std::string &getTitle() const;
  const std::string &getBody() const;
  // get comments
private:
  User user_;
  std::string title_;
  std::string body_;
  // add other stuff
  std::vector<Comment> comments_;
};
