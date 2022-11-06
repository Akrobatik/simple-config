// #include "config.h"
#include "config.h"

// #include <fstream>
#include <fstream>
// #include <regex>
#include <regex>

namespace akrbt {
namespace config {
Value::Value() : value_(std::make_unique<details::_Null>()) {}
Value::Value(const std::string& value) : value_(std::make_unique<details::_String>(value)) {}
Value::Value(int32_t value) : value_(std::make_unique<details::_Number>(value)) {}
Value::Value(int64_t value) : value_(std::make_unique<details::_Number>(value)) {}
Value::Value(uint32_t value) : value_(std::make_unique<details::_Number>(value)) {}
Value::Value(uint64_t value) : value_(std::make_unique<details::_Number>(value)) {}
Value::Value(double value) : value_(std::make_unique<details::_Number>(value)) {}
Value::Value(bool value) : value_(std::make_unique<details::_Boolean>(value)) {}

Value::Value(std::unique_ptr<details::_Value> value) : value_(std::move(value)) {}
Value::Value(const Value& other) : value_(other.value_->Copy()) {}
Value::Value(Value&& other) noexcept : value_(std::move(other.value_)) {}

bool Value::is_null() const { return value_->is_null(); }
bool Value::is_string() const { return value_->is_string(); }
bool Value::is_number() const { return value_->is_number(); }
bool Value::is_boolean() const { return value_->is_boolean(); }
bool Value::is_array() const { return value_->is_array(); }
bool Value::is_object() const { return value_->is_object(); }

bool Value::has_field(const std::string& key) const { return value_->has_field(key); }
bool Value::has_string_field(const std::string& key) const { return value_->has_field(key) && value_->as_object().at(key).is_string(); }
bool Value::has_number_field(const std::string& key) const { return value_->has_field(key) && value_->as_object().at(key).is_number(); }
bool Value::has_boolean_field(const std::string& key) const { return value_->has_field(key) && value_->as_object().at(key).is_boolean(); }
bool Value::has_array_field(const std::string& key) const { return value_->has_field(key) && value_->as_object().at(key).is_array(); }
bool Value::has_object_field(const std::string& key) const { return value_->has_field(key) && value_->as_object().at(key).is_object(); }

std::string Value::as_string() const { return value_->as_string(); }
int32_t Value::as_integer() const { return value_->as_number().to_int32(); }
double Value::as_double() const { return value_->as_number().to_double(); }
const Number& Value::as_number() const { return value_->as_number(); }
bool Value::as_boolean() const { return value_->as_boolean(); }
Array& Value::as_array() { return value_->as_array(); }
const Array& Value::as_array() const { return value_->as_array(); }
Object& Value::as_object() { return value_->as_object(); }
const Object& Value::as_object() const { return value_->as_object(); }

Value& Value::operator=(const Value& other) {
  if (this != &other) {
    value_ = std::unique_ptr<details::_Value>(other.value_->Copy());
  }

  return *this;
}

Value& Value::operator=(Value&& other) noexcept {
  if (this != &other) {
    value_.swap(other.value_);
  }

  return *this;
}

Value& Value::operator[](size_t index) {
  if (this->is_null()) {
    value_.reset(new details::_Array());
  }

  return value_->Get(index);
}

Value& Value::operator[](const std::string& key) {
  if (this->is_null()) {
    value_.reset(new details::_Object());
  }

  return value_->Get(key);
}

void Value::Save(const std::string& file_path) {
  std::ofstream output_file(file_path, std::ios::trunc);
  if (!output_file.is_open()) {
    return;
  }

  value_->Format(output_file, 0);

  output_file.close();
}

Value Value::Load(const std::string& file_path) {
  static const std::regex REGEX_SEARCH("(<.+>)");
  static const std::regex REGEX_PARENT_NEW("<([^/#\\s]+)>");
  static const std::regex REGEX_PARENT_END("<(/[^#\\s]+)>");
  static const std::regex REGEX_ARRAY_NEW("<#Array>");
  static const std::regex REGEX_ARRAY_END("<Array#>");
  static const std::regex REGEX_OBJECT_NEW("<#Object>");
  static const std::regex REGEX_OBJECT_END("<Object#>");
  static const std::regex REGEX_DATA("<key=\\\"([^\\\"]+)\\\"\\s+type=\\\"([^\\\"]+)\\\"\\s+value=\\\"([^\\\"]+)\\\">");
  static const std::regex REGEX_DATA_NO_KEY("<type=\\\"([^\\\"]+)\\\"\\s+value=\\\"([^\\\"]+)\\\">");

  std::ifstream intput_file(file_path);
  if (!intput_file.is_open()) {
    return null();
  }

  Value new_value = null();

  std::vector<std::pair<std::string, Value*>> parents;

  std::string line;
  while (!intput_file.eof()) {
    std::getline(intput_file, line);

    std::smatch search_matches;
    if (std::regex_search(line, search_matches, REGEX_SEARCH)) {
      std::string data = search_matches[1];
      std::smatch data_matches;

      if (std::regex_match(data, data_matches, REGEX_PARENT_NEW)) {
        if (parents.empty()) {
          Value* backup = &new_value[data_matches[1]];
          parents.push_back(std::pair<std::string, Value*>(data_matches[1], backup));
        } else {
          Value* backup = &(*parents.back().second)[data_matches[1]];
          parents.push_back(std::pair<std::string, Value*>(data_matches[1], backup));
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_PARENT_END)) {
        if (parents.empty()) {
          throw Exception("invalid config format");
        } else {
          parents.pop_back();
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_ARRAY_NEW)) {
        if (!parents.back().second->is_array()) {
          *parents.back().second = array();
        }

        Array::size_type size = parents.back().second->as_array().size();

        if (parents.empty()) {
          new_value[size] = array();
          Value* backup = &new_value[size];
          parents.push_back(std::pair<std::string, Value*>(data_matches[1], backup));
        } else {
          (*parents.back().second)[size] = array();
          Value* backup = &(*parents.back().second)[size];
          parents.push_back(std::pair<std::string, Value*>(data_matches[1], backup));
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_ARRAY_END)) {
        if (parents.empty()) {
          throw Exception("invalid config format");
        } else {
          parents.pop_back();
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_OBJECT_NEW)) {
        if (!parents.back().second->is_array()) {
          *parents.back().second = array();
        }

        Array::size_type size = parents.back().second->as_array().size();

        if (parents.empty()) {
          new_value[size] = object();
          Value* backup = &new_value[size];
          parents.push_back(std::pair<std::string, Value*>(data_matches[1], backup));
        } else {
          (*parents.back().second)[size] = object();
          Value* backup = &(*parents.back().second)[size];
          parents.push_back(std::pair<std::string, Value*>(data_matches[1], backup));
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_OBJECT_END)) {
        if (parents.empty()) {
          throw Exception("invalid config format");
        } else {
          parents.pop_back();
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_DATA)) {
        if (parents.empty()) {
          if (data_matches[2].compare("String") == 0) {
            new_value[data_matches[1]] = string(data_matches[3]);
            Value* backup = &new_value[data_matches[1]];
          } else if (data_matches[2].compare("Number") == 0) {
            if (data_matches[3].str().find(".") != std::string::npos) {
              new_value[data_matches[1]] = number(std::stoll(data_matches[3]));
            } else {
              new_value[data_matches[1]] = number(std::stod(data_matches[3]));
            }

            Value* backup = &new_value[data_matches[1]];
          } else if (data_matches[2].compare("Boolean") == 0) {
            new_value[data_matches[1]] = boolean(data_matches[3].compare("false"));
            Value* backup = &new_value[data_matches[1]];
          } else {
            throw Exception("invalid config format");
          }
        } else {
          if (data_matches[2].compare("String") == 0) {
            (*parents.back().second)[data_matches[1]] = string(data_matches[3]);
            Value* backup = &(*parents.back().second)[data_matches[1]];
          } else if (data_matches[2].compare("Number") == 0) {
            if (data_matches[3].str().find(".") != std::string::npos) {
              (*parents.back().second)[data_matches[1]] = number(std::stoll(data_matches[3]));
            } else {
              (*parents.back().second)[data_matches[1]] = number(std::stod(data_matches[3]));
            }

            Value* backup = &(*parents.back().second)[data_matches[1]];
          } else if (data_matches[2].compare("Boolean") == 0) {
            (*parents.back().second)[data_matches[1]] = boolean(data_matches[3].compare("false"));
            Value* backup = &(*parents.back().second)[data_matches[1]];
          } else {
            throw Exception("invalid config format");
          }
        }

        continue;
      }

      if (std::regex_match(data, data_matches, REGEX_DATA_NO_KEY)) {
        if (parents.empty()) {
          throw Exception("invalid config format");
        } else {
          if (!parents.back().second->is_array()) {
            throw Exception("invalid config format");
          }

          Array::size_type size = parents.back().second->as_array().size();

          if (data_matches[1].compare("String") == 0) {
            (*parents.back().second)[size] = string(data_matches[2]);
            Value* backup = &(*parents.back().second)[size];
          } else if (data_matches[1].compare("Number") == 0) {
            if (data_matches[3].str().find(".") != std::string::npos) {
              (*parents.back().second)[size] = number(std::stoll(data_matches[2]));
            } else {
              (*parents.back().second)[size] = number(std::stod(data_matches[2]));
            }

            Value* backup = &(*parents.back().second)[data_matches[1]];
          } else if (data_matches[1].compare("Boolean") == 0) {
            (*parents.back().second)[size] = boolean(data_matches[2].compare("false"));
            Value* backup = &(*parents.back().second)[size];
          } else {
            throw Exception("invalid config format");
          }
        }

        continue;
      }
    }
  }

  intput_file.close();

  return new_value;
}

Value Value::null() { return Value(); }
Value Value::string(const std::string& value) { return Value(value); }
Value Value::number(int32_t value) { return Value(value); }
Value Value::number(int64_t value) { return Value(value); }
Value Value::number(uint32_t value) { return Value(value); }
Value Value::number(uint64_t value) { return Value(value); }
Value Value::number(double value) { return Value(value); }
Value Value::boolean(bool value) { return Value(value); }
Value Value::array() { return Value(std::move(std::make_unique<details::_Array>())); }
Value Value::array(size_t size) { return Value(std::move(std::make_unique<details::_Array>(size))); }
Value Value::array(std::vector<Value> elements) { return Value(std::move(std::make_unique<details::_Array>(elements))); }
Value Value::object() { return Value(std::move(std::make_unique<details::_Object>())); }
Value Value::object(std::vector<std::pair<std::string, Value>> elements) { return Value(std::move(std::make_unique<details::_Object>(elements))); }
}  // namespace config
}  // namespace akrbt