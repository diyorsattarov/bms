#pragma once

#include "client/session.hpp"
#include <memory>
#include <string>

class User {
public:
  User(const std::string &username);
  std::string getUsername() const;

private:
  std::string username_;
};

class Service {
public:
  virtual ~Service() = default;
};

class UserService {
public:
  void addUser(const std::string &username);
  int getUsersSize() const;
  void printUsers();

private:
  std::vector<User> users_;
};

class ClientService : public Service {
public:
  std::string makeRequest(const std::string &host, const std::string &port,
                          const std::string &target);

  std::string makeRequest(const std::string &host, const std::string &port,
                          const std::string &target, const std::string &body,
                          http::verb method = http::verb::get);
};

class Application {
public:
  Application();
  virtual ~Application() = default;

  std::shared_ptr<ClientService> getClientService() const;
  std::shared_ptr<UserService> getUserService() const;

private:
  std::shared_ptr<ClientService> clientService_;
  std::shared_ptr<UserService> userService_;
};
