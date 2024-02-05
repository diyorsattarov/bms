#include "../include/application.hpp"
#include <future>
Application::Application()
    : clientService_(std::make_shared<ClientService>()) {}

std::shared_ptr<ClientService> Application::getClientService() const {
  return clientService_;
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

  // Use a promise to signal when the request is complete
  std::promise<std::string> responsePromise;

  auto session_ptr = std::make_shared<session>(client_ioc);
  session_ptr->run(host, port, target, body, method,
                   [&responsePromise](std::string response) {
                     // Set the response to the promise when the request is
                     // complete
                     responsePromise.set_value(response);
                   });

  // Run the IO context asynchronously
  std::thread([&client_ioc]() { client_ioc.run(); }).detach();

  // Wait for the response from the promise
  std::future<std::string> responseFuture = responsePromise.get_future();
  return responseFuture.get();
}
