#include "../include/user.hpp"

User::User(const std::string &username) : username_(username) {}

std::string User::getUsername() const { return username_; }
