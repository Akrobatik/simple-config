#pragma once

// #include <exception>
#include <exception>
// #include <string>
#include <string>

namespace akrbt {
namespace config {
class Exception : public std::exception {
 public:
  Exception(const std::string& message) : message_(message) {}

  virtual const char* what() const { return message_.c_str(); }

 private:
  std::string message_;
};
}  // namespace config
}  // namespace akrbt