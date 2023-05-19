#pragma once

#include <memory>
#include <vector>
#include <map>
#include <void/object.hpp>

namespace Void {
  extern std::map<std::string, std::shared_ptr<Builtin>> builtin_func_map;

  extern std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> const&);
  extern std::shared_ptr<Object> first(std::vector<std::shared_ptr<Object>> const&);
  extern std::shared_ptr<Object> last(std::vector<std::shared_ptr<Object>> const&);
  extern std::shared_ptr<Object> push(std::vector<std::shared_ptr<Object>> const&);
  extern std::shared_ptr<Object> pop(std::vector<std::shared_ptr<Object>> const&);
  extern std::shared_ptr<Object> puts(std::vector<std::shared_ptr<Object>> const&);
}
