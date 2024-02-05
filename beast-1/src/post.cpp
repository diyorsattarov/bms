#include "../include/post.hpp"
Comment::Comment(const User &user, const std::string &body)
    : user_(user), body_(body) {}

Post::Post(const User &user, const std::string &title, const std::string &body)
    : user_(user), title_(title), body_(body) {}

std::vector<Comment> &Post::getComments() { return comments_; }

void Post::addComment(const User &user, const std::string &body) {
  comments_.emplace_back(user, body);
}

const User &Post::getPostUser() const { return user_; }

const std::string &Post::getTitle() const { return title_; }

const std::string &Post::getBody() const { return body_; }
