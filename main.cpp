#include <iostream>
#include <boost/asio.hpp>

int main() {
  boost::asio::io_service io;
  boost::asio::deadline_timer deadlineTimer;
  std::cout << "Hello, World!" << std::endl;
  return 0;
}