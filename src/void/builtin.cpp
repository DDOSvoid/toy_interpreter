#include <cstddef>
#include <system_error>
#include <void/builtin.hpp>
#include <void/object.hpp>
#include <memory>
#include <vector>
#include <iostream>

namespace Void {
  std::map<std::string, std::shared_ptr<Builtin>> builtin_func_map = {
    {"len", std::make_shared<Builtin>(len, "len")},
    {"first", std::make_shared<Builtin>(first, "first")},
    {"last", std::make_shared<Builtin>(last, "last")},
    {"push", std::make_shared<Builtin>(push, "push")},
    {"pop", std::make_shared<Builtin>(pop, "pop")},
    {"puts", std::make_shared<Builtin>(puts, "puts")},
  };

  std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> const& args) {
    if (args.size() != 1) {
      return std::make_shared<Error>();
    }
    
    auto& obj = args[0];

    if (obj->type() == Object::string_object_t) {
      return std::make_shared<Integer>(obj->cast<String>()->value().size());
    } else if (obj->type() == Object::array_object_t) {
      return std::make_shared<Integer>(obj->cast<Array>()->value().size());
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> first(std::vector<std::shared_ptr<Object>> const& args) {
    if (args.size() != 1) {
      return std::make_shared<Error>();
    }
    
    auto& obj = args[0];

    if (obj->type() == Object::array_object_t) {
      auto arr = obj->cast<Array>();
      if (arr->elements().empty()) {
	return std::make_shared<Error>();
      } else { 
	return arr->elements().front();
      }
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> last(std::vector<std::shared_ptr<Object>> const& args) {
    if (args.size() != 1) {
      return std::make_shared<Error>();
    }
    
    auto& obj = args[0];

    if (obj->type() == Object::array_object_t) {
      auto arr = obj->cast<Array>();
      if (arr->elements().empty()) {
	return std::make_shared<Error>();
      } else { 
	return arr->elements().back();
      }
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> push(std::vector<std::shared_ptr<Object>> const& args) {
    if (args.size() != 2) {
      return std::make_shared<Error>();
    }

    auto& arr_obj = args[0];
    auto& obj = args[1];

    if (arr_obj->type() == Object::array_object_t) {
      auto& elems = arr_obj->cast<Array>()->elements();
      auto res = std::make_shared<Array>();
      for (auto& elem : elems) {
	res->append(elem);
      }
      res->append(obj);
      return res; 
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> pop(std::vector<std::shared_ptr<Object>> const& args) {
    if (args.size() != 1) {
      return std::make_shared<Error>();
    }

    auto& arr_obj = args[0];

    if (arr_obj->type() == Object::array_object_t) {
      auto& elems = arr_obj->cast<Array>()->elements();
      if (elems.empty()) {
	return std::make_shared<Error>();
      }
      auto res = std::make_shared<Array>();
      for (std::size_t i = 0; i + 1 < elems.size(); ++i) {
	res->append(elems[i]);
      }
      return res; 
    } else {
      return std::make_shared<Error>();
    }
  }

  std::shared_ptr<Object> puts(std::vector<std::shared_ptr<Object>> const& args) {
    if (args.size() != 1) {
      return std::make_shared<Error>();
    }

    std::cout << "<puts: " << args[0]->inspect() << ">" << std::endl;

    return null_obj;
  }
}
