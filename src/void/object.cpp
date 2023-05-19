#include <vector>
#include <void/ast.hpp>
#include <memory>
#include <type_traits>
#include <void/object.hpp>

namespace Void {
  // Object
  Object::Object(ObjectType type) : _type(type) {}

  Object::ObjectType Object::type() const {
    return _type;
  }

  // Integer
  Integer::Integer(int value)
    : Object(ObjectType::integer_object_t),
      _value(value)
  {}

  std::string Integer::inspect() const {
    return std::to_string(_value);
  }

  int Integer::value() const {
    return _value;
  } 

  // Boolean
  Boolean::Boolean(bool value)
    : Object(ObjectType::boolean_object_t),
      _value(value)
  {}

  std::string Boolean::inspect() const {
    return _value ? "true" : "false";
  }

  bool Boolean::value() const {
    return _value;
  }

  // String
  String::String(std::string value)
    : Object(ObjectType::string_object_t),
      _value(value)
  {}

  std::string String::inspect() const {
    return _value;
  }

  std::string String::value() const {
    return _value;
  }

  // Return
  Return::Return(std::shared_ptr<Object> obj)
    :Object(ObjectType::null_object_t),
     _value(std::move(obj))
  {}

  std::string Return::inspect() const {
    return _value->inspect();
  }

  std::shared_ptr<Object> Return::value() const {
    return _value;
  }

  // Error
  Error::Error()
    : Object(ObjectType::error_object_t) {}
  
  Error::Error(std::string value)
    : Object(ObjectType::error_object_t),
      _value(value)
  {}

  std::string Error::inspect() const {
    return "<error: " + _value + ">";
  }

  std::string Error::value() const {
    return _value;
  }

  // Null
  Null::Null()
    : Object(ObjectType::null_object_t)
  {}

  std::string Null::inspect() const {
    return "null"; 
  }

  // Function
  Function::Function(FunctionLiteral* func, Environment* env)
    : Object(ObjectType::function_object_t),
      _env(std::make_unique<Environment>(env)),
      _function(func)
  {}

  std::string Function::inspect() const {
    return _function->to_string();
  }

  FunctionLiteral* Function::value() const {
    return _function;
  }

  FunctionLiteral* Function::function() const {
    return _function;
  }

  Environment* Function::env() const {
    return _env.get();
  }

  // Array
  Array::Array()
    : Object(ObjectType::array_object_t)
  {}

  std::string Array::inspect() const {
    std::string res;
    bool first = false;
    for (auto& elem : _elements) {
      if (first) {
	res += ", ";
      }
      first = true;
      res += elem->inspect(); 
    }
    return "[" + res + "]";
  }

  std::vector<std::shared_ptr<Object>> const& Array::value() const {
    return _elements; 
  }
  
  std::vector<std::shared_ptr<Object>> const& Array::elements() const {
    return _elements; 
  }

  void Array::append(std::shared_ptr<Object> obj) {
    _elements.emplace_back(std::move(obj)); 
  }

  // Builtin
  Builtin::Builtin(BuiltinFunction fn, std::string const& name)
    : Object(ObjectType::builtin_object_t),
      _function(fn),
      _name(name)
  {}

  std::string Builtin::inspect() const {
    return "<builtin: " + _name + ">";
  }

  std::shared_ptr<Object> Builtin::run(std::vector<std::shared_ptr<Object>> const& args) {
    return _function(args);
  }
  

  std::shared_ptr<Null> null_obj = std::make_shared<Null>();
  std::shared_ptr<Boolean> true_obj = std::make_shared<Boolean>(true);
  std::shared_ptr<Boolean> false_obj = std::make_shared<Boolean>(false);

  // Environment
  Environment::Environment()
    : _outer(nullptr)
  {}
  
  Environment::Environment(Environment* outer)
    : _outer(outer)
  {}

  std::shared_ptr<Object> Environment::get(std::string name) {
    auto it = _store.find(name);
    if (it != _store.end()) {
      return it->second;
    } else if (_outer) {
      auto res = _outer->get(name);
      if (res->type() != Object::null_object_t) {
	return res;
      }
    }
    return null_obj; 
  }

  void Environment::set(std::string name, std::shared_ptr<Object> obj) {
    _store[name] = std::move(obj); 
  }
}
