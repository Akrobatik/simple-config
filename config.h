#pragma once

// #include <iostream>
#include <iostream>
// #include <memory>
#include <memory>
// #include <string>
#include <string>
// #include <utility>
#include <utility>
// #include <vector>
#include <vector>

// #include "config-exception.h"
#include "config-exception.h"

namespace akrbt {
namespace config {
namespace details {
class _Value;
class _Null;
class _String;
class _Number;
class _Boolean;
class _Array;
class _Object;
}  // namespace details

class Value;
class Number;
class Array;
class Object;

class Value {
 public:
  Value();
  explicit Value(const std::string& value);
  explicit Value(int32_t value);
  explicit Value(int64_t value);
  explicit Value(uint32_t value);
  explicit Value(uint64_t value);
  explicit Value(double value);
  explicit Value(bool value);

  Value(const Value& other);
  Value(Value&& other) noexcept;

  bool is_null() const;
  bool is_string() const;
  bool is_number() const;
  bool is_boolean() const;
  bool is_array() const;
  bool is_object() const;

  std::string as_string() const;
  int32_t as_integer() const;
  double as_double() const;
  const Number& as_number() const;
  bool as_boolean() const;
  Array& as_array();
  const Array& as_array() const;
  Object& as_object();
  const Object& as_object() const;

  Value& operator=(const Value& other);
  Value& operator=(Value&& other) noexcept;

  Value& operator[](size_t index);
  Value& operator[](const std::string& key);

  void Save(const std::string& file_path);
  static Value Load(const std::string& file_path);

  static Value null();
  static Value string(const std::string& value);
  static Value number(int32_t value);
  static Value number(int64_t value);
  static Value number(uint32_t value);
  static Value number(uint64_t value);
  static Value number(double value);
  static Value boolean(bool value);
  static Value array();
  static Value array(size_t size);
  static Value array(std::vector<Value> elements);
  static Value object();
  static Value object(std::vector<std::pair<std::string, Value>> elements);

 private:
  friend class details::_Array;
  friend class details::_Object;

  explicit Value(std::unique_ptr<details::_Value> value);

  std::unique_ptr<details::_Value> value_;
};

class Number {
 public:
  int32_t to_int32() const {
    if (type_ == Type::DOUBLE)
      return static_cast<int32_t>(double_value_);
    else
      return static_cast<int32_t>(int64_value_);
  }

  uint32_t to_uint32() const {
    if (type_ == Type::DOUBLE)
      return static_cast<uint32_t>(double_value_);
    else
      return static_cast<uint32_t>(int64_value_);
  }

  int64_t to_int64() const {
    if (type_ == Type::DOUBLE)
      return static_cast<int64_t>(double_value_);
    else
      return static_cast<int64_t>(int64_value_);
  }

  uint64_t to_uint64() const {
    if (type_ == Type::DOUBLE)
      return static_cast<uint64_t>(double_value_);
    else
      return static_cast<uint64_t>(int64_value_);
  }

  double to_double() const {
    switch (type_) {
      case Type::SIGNED:
        return static_cast<double>(int64_value_);
        break;

      case Type::UNSIGNED:
        return static_cast<double>(uint64_value_);
        break;

      case Type::DOUBLE:
        return double_value_;
        break;
    }
  }

 private:
  friend class details::_Number;

  enum class Type {
    SIGNED,
    UNSIGNED,
    DOUBLE,
  };

  Number(int32_t value) : type_(Type::SIGNED), int64_value_(value) {}
  Number(int64_t value) : type_(Type::SIGNED), int64_value_(value) {}
  Number(uint32_t value) : type_(Type::UNSIGNED), uint64_value_(value) {}
  Number(uint64_t value) : type_(Type::UNSIGNED), uint64_value_(value) {}
  Number(double value) : type_(Type::DOUBLE), double_value_(value) {}

  Type type_;

  union {
    int64_t int64_value_;
    uint64_t uint64_value_;
    double double_value_;
  };
};

class Array {
 public:
  typedef std::vector<Value> StorageType;
  typedef StorageType::iterator iterator;
  typedef StorageType::const_iterator const_iterator;
  typedef StorageType::reverse_iterator reverse_iterator;
  typedef StorageType::const_reverse_iterator const_reverse_iterator;
  typedef StorageType::size_type size_type;

  iterator begin() { return elements_.begin(); }
  const_iterator begin() const { return elements_.cbegin(); }
  iterator end() { return elements_.end(); }
  const_iterator end() const { return elements_.cend(); }
  reverse_iterator rbegin() { return elements_.rbegin(); }
  const_reverse_iterator rbegin() const { return elements_.rbegin(); }
  reverse_iterator rend() { return elements_.rend(); }
  const_reverse_iterator rend() const { return elements_.crend(); }
  const_iterator cbegin() const { return elements_.cbegin(); }
  const_iterator cend() const { return elements_.cend(); }
  const_reverse_iterator crbegin() const { return elements_.crbegin(); }
  const_reverse_iterator crend() const { return elements_.crend(); }

  iterator erase(iterator iter) { return elements_.erase(iter); }

  void erase(size_type index) {
    if (index >= elements_.size()) {
      throw Exception("index out of bounds");
    }

    elements_.erase(elements_.begin() + index);
  }

  Value& at(size_type index) {
    if (index >= elements_.size()) {
      throw Exception("index out of bounds");
    }

    return elements_[index];
  }

  const Value& at(size_type index) const {
    if (index >= elements_.size()) {
      throw Exception("index out of bounds");
    }

    return elements_[index];
  }

  size_type size() const { return elements_.size(); }

  Value& operator[](size_type index) {
    if (index >= elements_.size()) {
      elements_.resize(index + 1);
    }

    return elements_[index];
  }

 private:
  friend class details::_Array;

  Array() : elements_() {}
  Array(size_type size) : elements_(size) {}
  Array(StorageType elements) : elements_(std::move(elements)) {}

  StorageType elements_;
};

class Object {
 public:
  typedef std::vector<std::pair<std::string, Value>> StorageType;
  typedef StorageType::iterator iterator;
  typedef StorageType::const_iterator const_iterator;
  typedef StorageType::reverse_iterator reverse_iterator;
  typedef StorageType::const_reverse_iterator const_reverse_iterator;
  typedef StorageType::size_type size_type;

  iterator begin() { return elements_.begin(); }
  const_iterator begin() const { return elements_.cbegin(); }
  iterator end() { return elements_.end(); }
  const_iterator end() const { return elements_.cend(); }
  reverse_iterator rbegin() { return elements_.rbegin(); }
  const_reverse_iterator rbegin() const { return elements_.rbegin(); }
  reverse_iterator rend() { return elements_.rend(); }
  const_reverse_iterator rend() const { return elements_.crend(); }
  const_iterator cbegin() const { return elements_.cbegin(); }
  const_iterator cend() const { return elements_.cend(); }
  const_reverse_iterator crbegin() const { return elements_.crbegin(); }
  const_reverse_iterator crend() const { return elements_.crend(); }

  iterator erase(iterator iter) { return elements_.erase(iter); }

  void erase(const std::string& key) {
    iterator iter = FindByKey(key);

    if (iter == elements_.end()) {
      throw Exception("Key not found");
    }

    elements_.erase(iter);
  }

  Value& at(const std::string& key) {
    iterator iter = FindByKey(key);

    if (iter == elements_.end()) {
      throw Exception("Key not found");
    }

    return iter->second;
  }

  const Value& at(const std::string& key) const {
    const_iterator iter = FindByKey(key);

    if (iter == elements_.end()) {
      throw Exception("Key not found");
    }

    return iter->second;
  }

  Value& operator[](const std::string& key) {
    iterator iter = FindInsertLocation(key);

    if (iter == elements_.end() || key != iter->first) {
      return elements_.insert(iter, std::pair<std::string, Value>(key, Value()))->second;
    }

    return iter->second;
  }

 private:
  friend class details::_Object;

  Object() : elements_() {}
  Object(StorageType elements) : elements_(std::move(elements)) {}

  iterator FindInsertLocation(const std::string& key) {
    return std::find_if(elements_.begin(), elements_.end(), [&key](const std::pair<std::string, Value>& element) -> bool {
      return element.first == key;
    });
  }

  const_iterator FindByKey(const std::string& key) const {
    return std::find_if(elements_.begin(), elements_.end(), [&key](const std::pair<std::string, Value>& element) -> bool {
      return element.first == key;
    });
  }

  iterator FindByKey(const std::string& key) {
    iterator iter = FindInsertLocation(key);

    if (iter != elements_.end() && key != iter->first) {
      return elements_.end();
    }

    return iter;
  }

  StorageType elements_;
};

namespace details {
class _Value {
 public:
  virtual bool is_null() const { return false; }
  virtual bool is_string() const { return false; }
  virtual bool is_number() const { return false; }
  virtual bool is_boolean() const { return false; }
  virtual bool is_array() const { return false; }
  virtual bool is_object() const { return false; }

  virtual std::string as_string() const { throw Exception("not a string"); };
  virtual const Number& as_number() const { throw Exception("not a number"); };
  virtual int as_integer() const { throw Exception("not a number"); };
  virtual double as_double() const { throw Exception("not a number"); };
  virtual bool as_boolean() const { throw Exception("not a boolean"); };
  virtual Array& as_array() { throw Exception("not an array"); };
  virtual const Array& as_array() const { throw Exception("not an array"); };
  virtual Object& as_object() { throw Exception("not an object"); };
  virtual const Object& as_object() const { throw Exception("not an object"); };

  virtual std::unique_ptr<_Value> Copy() = 0;

  virtual Value& Get(size_t index) { throw Exception("not an array"); }
  virtual const Value& Get(size_t index) const { throw Exception("not an array"); }
  virtual Value& Get(const std::string& key) { throw Exception("not an object"); }
  virtual const Value& Get(const std::string& key) const { throw Exception("not an object"); }

  virtual void Format(std::ostream& out, int indent) const {}
  virtual void Format(std::ostream& out, int indent, const std::string& key) const {}

  virtual ~_Value() {}

 protected:
  static const int INDENT_WIDTH = 2;

  _Value() {}
};

class _Null : public _Value {
 public:
  virtual bool is_null() const { return true; }

  virtual std::unique_ptr<_Value> Copy() { return std::make_unique<_Null>(); }
};

class _String : public _Value {
 public:
  _String(const std::string& value) : value_(value) {}

  virtual bool is_string() const { return true; }
  virtual std::string as_string() const { return value_; };

  virtual std::unique_ptr<_Value> Copy() { return std::make_unique<_String>(*this); }

  virtual void Format(std::ostream& out, int indent) const {
    std::string indent_string(indent, ' ');
    out << indent_string << "<type=\"String\" value=\"" << value_ << "\">" << std::endl;
  }

  virtual void Format(std::ostream& out, int indent, const std::string& key) const {
    std::string indent_string(indent, ' ');
    out << indent_string << "<key=\"" << key << "\" type=\"String\" value=\"" << value_ << "\">" << std::endl;
  }

 private:
  std::string value_;
};

class _Number : public _Value {
 public:
  _Number(int32_t value) : number_(value) {}
  _Number(int64_t value) : number_(value) {}
  _Number(uint32_t value) : number_(value) {}
  _Number(uint64_t value) : number_(value) {}
  _Number(double value) : number_(value) {}

  virtual bool is_number() const { return true; }
  virtual const Number& as_number() const { return number_; };
  virtual int as_integer() const { return number_.to_int32(); };
  virtual double as_double() const { return number_.to_double(); };

  virtual std::unique_ptr<_Value> Copy() { return std::make_unique<_Number>(*this); }

  virtual void Format(std::ostream& out, int indent) const {
    std::string indent_string(indent, ' ');

    switch (number_.type_) {
      case Number::Type::SIGNED:
        out << indent_string << "<type=\"Number\" value=\"" << number_.int64_value_ << "\">" << std::endl;
        break;

      case Number::Type::UNSIGNED:
        out << indent_string << "<type=\"Number\" value=\"" << number_.uint64_value_ << "\">" << std::endl;
        break;

      case Number::Type::DOUBLE:
        out << indent_string << "<type=\"Number\" value=\"" << number_.double_value_ << "\">" << std::endl;
        break;
    }
  }

  virtual void Format(std::ostream& out, int indent, const std::string& key) const {
    std::string indent_string(indent, ' ');

    switch (number_.type_) {
      case Number::Type::SIGNED:
        out << indent_string << "<key=\"" << key << "\" type=\"Number\" value=\"" << number_.int64_value_ << "\">" << std::endl;
        break;

      case Number::Type::UNSIGNED:
        out << indent_string << "<key=\"" << key << "\" type=\"Number\" value=\"" << number_.uint64_value_ << "\">" << std::endl;
        break;

      case Number::Type::DOUBLE:
        out << indent_string << "<key=\"" << key << "\" type=\"Number\" value=\"" << number_.double_value_ << "\">" << std::endl;
        break;
    }
  }

 private:
  Number number_;
};

class _Boolean : public _Value {
 public:
  _Boolean(bool value) : value_(value) {}

  virtual bool is_boolean() const { return true; }
  virtual bool as_boolean() const { return value_; };

  virtual std::unique_ptr<_Value> Copy() { return std::make_unique<_Boolean>(*this); }

  virtual void Format(std::ostream& out, int indent) const {
    std::string indent_string(indent, ' ');
    out << indent_string << "<type=\"Boolean\" value=\"" << std::boolalpha << value_ << "\">" << std::endl;
  }

  virtual void Format(std::ostream& out, int indent, const std::string& key) const {
    std::string indent_string(indent, ' ');
    out << indent_string << "<key=\"" << key << "\" type=\"Boolean\" value=\"" << std::boolalpha << value_ << "\">" << std::endl;
  }

 private:
  bool value_;
};

class _Array : public _Value {
 public:
  _Array() : array_() {}
  _Array(Array::size_type size) : array_(size) {}
  _Array(Array::StorageType elements) : array_(std::move(elements)) {}

  virtual bool is_array() const { return true; }
  virtual Array& as_array() { return array_; };
  virtual const Array& as_array() const { return array_; };

  virtual std::unique_ptr<_Value> Copy() { return std::make_unique<_Array>(*this); }

  virtual Value& Get(size_t index) { return array_[index]; }

  virtual void Format(std::ostream& out, int indent) const {
    std::string indent_string(indent, ' ');

    for (auto& element : array_) {
      if (element.is_null()) {
        continue;
      }

      if (element.is_array()) {
        out << indent_string << "<#Array>" << std::endl;
        element.value_.get()->Format(out, indent + INDENT_WIDTH);
        out << indent_string << "<Array#>" << std::endl;
      } else if (element.is_object()) {
        out << indent_string << "<#Object>" << std::endl;
        element.value_.get()->Format(out, indent + INDENT_WIDTH);
        out << indent_string << "<Object#>" << std::endl;
      } else {
        element.value_.get()->Format(out, indent);
      }
    }
  }

 private:
  Array array_;
};

class _Object : public _Value {
 public:
  _Object() : object_() {}
  _Object(Object::StorageType elements) : object_(std::move(elements)) {}

  virtual bool is_object() const { return true; }
  virtual Object& as_object() { return object_; };
  virtual const Object& as_object() const { return object_; };

  virtual std::unique_ptr<_Value> Copy() { return std::make_unique<_Object>(*this); }

  virtual Value& Get(const std::string& key) { return object_[key]; }

  virtual void Format(std::ostream& out, int indent) const {
    std::string indent_string(indent, ' ');

    for (auto& element : object_) {
      if (element.second.is_null()) {
        continue;
      }

      if (element.second.is_array() || element.second.is_object()) {
        out << indent_string << "<" << element.first << ">" << std::endl;
        element.second.value_.get()->Format(out, indent + INDENT_WIDTH);
        out << indent_string << "</" << element.first << ">" << std::endl;
      } else {
        element.second.value_.get()->Format(out, indent, element.first);
      }
    }
  }

 private:
  friend class Value;

  Object object_;
};
}  // namespace details
}  // namespace config
}  // namespace akrbt