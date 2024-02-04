#include "../include/application.hpp"

User::User(const std::string &username) : username_(username) {}

std::string User::getUsername() const { return username_; }

Application::Application()
    : clientService_(std::make_shared<ClientService>()),
      userService_(std::make_shared<UserService>()) {}

std::shared_ptr<ClientService> Application::getClientService() const {
  return clientService_;
}

std::shared_ptr<UserService> Application::getUserService() const {
  return userService_;
}
std::string ClientService::makeRequest(const std::string &host,
                                       const std::string &port,
                                       const std::string &target) {
  net::io_context client_ioc; // Separate io_context for the client
  auto session_ptr = std::make_shared<session>(client_ioc);
  session_ptr->run(host, port, target);
  client_ioc.run();
  return session_ptr->getResponse();
}

std::string ClientService::makeRequest(const std::string &host,
                                       const std::string &port,
                                       const std::string &target,
                                       const std::string &body,
                                       http::verb method) {
  net::io_context client_ioc;
  auto session_ptr = std::make_shared<session>(client_ioc);
  session_ptr->run(host, port, target, body, method);
  client_ioc.run();
  return session_ptr->getResponse();
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
