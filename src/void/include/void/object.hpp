#pragma once

#include <void/token.hpp>
#include <void/lexer.hpp>
#include <void/ast.hpp>
#include <void/parser.hpp>

#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

namespace Void {
  class Object {
  public: 
    enum ObjectType {
      integer_object_t,
      boolean_object_t,
      string_object_t,
      error_object_t,
      null_object_t,
      return_object_t,
      function_object_t,
      array_object_t,
      builtin_object_t,
    };

    static std::map<ObjectType, std::string> t_to_s;

    explicit Object(ObjectType);
    virtual ~Object() {}

    template <typename T>
    T* cast() {
      return dynamic_cast<T*>(this); 
    }

    template <typename T>
    T const* cast() const {
      return dynamic_cast<T const*>(this);
    }

    ObjectType type() const;
    virtual std::string inspect() const = 0;

  private:
    ObjectType _type;
  };

  class Integer : public Object {
  public:
    explicit Integer(int);

    std::string inspect() const override;
    int value() const;
    
  private:
    int _value;
  };

  class Boolean : public Object {
  public:
    explicit Boolean(bool);

    std::string inspect() const override;
    bool value() const;

  private:
    bool _value;
  };

  class String : public Object {
  public:
    explicit String(std::string);

    std::string inspect() const override;
    std::string value() const;

  private:
    std::string _value;
  };

  class Return : public Object {
  public:
    explicit Return(std::shared_ptr<Object>);

    std::string inspect() const override;
    std::shared_ptr<Object> value() const;
    
  private:
    std::shared_ptr<Object> _value;
  };

  class Error : public Object {
  public:
    Error();
    explicit Error(std::string);

    std::string inspect() const override;
    std::string value() const;
    
  private:
    std::string _value; 
  };

  class Null : public Object {
  public:
    Null();
    
    std::string inspect() const override; 
  };

  class Environment; 
  class Function : public Object {
  public:
    Function(FunctionLiteral*, Environment*);

    std::string inspect() const override;
    FunctionLiteral* value() const;
    FunctionLiteral* function() const;
    Environment* env() const;
    
  private:
    FunctionLiteral* _function;
    std::unique_ptr<Environment> _env;
  };

  class Array : public Object {
  public:
    Array();

    std::string inspect() const override;
    std::vector<std::shared_ptr<Object>> const& elements() const;
    std::vector<std::shared_ptr<Object>> const& value() const;
    void append(std::shared_ptr<Object>); 
    
  private:
    std::vector<std::shared_ptr<Object>> _elements;
  };
  
  using BuiltinFunction = std::function<std::shared_ptr<Object>(std::vector<std::shared_ptr<Object>> const&)>;

  class Builtin : public Object {
  public:
    Builtin(BuiltinFunction, std::string const&);

    std::string inspect() const override; 
    std::shared_ptr<Object> run(std::vector<std::shared_ptr<Object>> const&);
    
  private:
    BuiltinFunction _function;
    std::string _name;
  };

  extern std::shared_ptr<Null> null_obj;
  extern std::shared_ptr<Boolean> true_obj;
  extern std::shared_ptr<Boolean> false_obj;
  
  class Environment {
  public:
    Environment(); 
    explicit Environment(Environment* outer);

    std::shared_ptr<Object> get(std::string); 
    void set(std::string, std::shared_ptr<Object>);
    
  private:
    std::map<std::string, std::shared_ptr<Object>> _store;
    Environment* _outer;
  };
}
