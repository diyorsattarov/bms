#pragma once

#include "client/session.hpp"
#include <memory>

class Service {
public:
  virtual ~Service() = default;
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

private:
  std::shared_ptr<ClientService> clientService_;
};
