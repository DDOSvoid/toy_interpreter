#pragma once

#include <void/token.hpp>

#include <string>
#include <vector>
#include <memory>

namespace Void {
  class AstNode {
  public:
    virtual std::string token_literal() const = 0; 
    virtual std::string to_string() const = 0; 
    virtual ~AstNode() {}
  };

  class Statement : public AstNode {
  public:
    Statement(Token);

    std::string token_literal() const override;
    
  protected:
    Token _token;
  };

  class Expression : public AstNode {
  public: 
    Expression(Token);

    std::string token_literal() const override;
    
  protected:
    Token _token;
  };

  class Program : public AstNode {
  public:
    std::string to_string() const override;
    std::string token_literal() const override;
    std::vector<std::unique_ptr<Statement>> const& statements() const;
    void append(std::unique_ptr<Statement>);

  private:
    std::vector<std::unique_ptr<Statement>> _statements;
  };

  class Identifier : public Expression {
  public:
    Identifier(Token);

    std::string to_string() const override;
    std::string value() const;
    
  private:
    std::string _value;
  };

  class LetStatement : public Statement {
  public: 
    using Statement::Statement;

    std::string to_string() const override;
    Identifier* identier() const;
    Expression* expression() const;
    void set_identifier(std::unique_ptr<Identifier>);
    void set_expression(std::unique_ptr<Expression>);
    
  private:
    std::unique_ptr<Identifier> _identifier;
    std::unique_ptr<Expression> _expression; 
  };

  class ReturnStatement : public Statement {
  public: 
    using Statement::Statement;

    std::string to_string() const override;
    Expression* expression() const;
    void set_expression(std::unique_ptr<Expression>);

  private:
    std::unique_ptr<Expression> _expression; 
  };

  class ExpressionStatement : public Statement {
  public:
    using Statement::Statement;

    std::string to_string() const override;
    Expression* expression() const;
    void set_expression(std::unique_ptr<Expression>);
    
  private:
    std::unique_ptr<Expression> _expression; 
  };

  class BlockStatement : public Statement {
  public:
    using Statement::Statement;

    std::string to_string() const override;
    std::vector<std::unique_ptr<Statement>> const& statements() const;
    void append(std::unique_ptr<Statement>);
  private:
    std::vector<std::unique_ptr<Statement>> _statements;
  };
  
  class IntegerLiteral : public Expression {
  public:
    IntegerLiteral(Token);

    std::string to_string() const override;
    int value() const; 
    
  private:
    int _value;
  };

  class BooleanLiteral : public Expression {
  public:
    BooleanLiteral(Token);

    std::string to_string() const override;
    bool value() const;
    
  private:
    bool _value;
  };

  class StringLiteral : public Expression {
  public:
    StringLiteral(Token);

    std::string to_string() const override;
    std::string value() const;
    
  private:
    std::string _value;
  };

  class ArrayLiteral : public Expression {
  public:
    using Expression::Expression;

    std::string to_string() const override;
    std::vector<std::unique_ptr<Expression>> const& expressions() const;
    void append(std::unique_ptr<Expression>); 
    
  private:
    std::vector<std::unique_ptr<Expression>> _expressions;
  };

  class FunctionLiteral : public Expression {
  public:
    using Expression::Expression;

    std::string to_string() const override;
    std::vector<std::unique_ptr<Identifier>> const& parameters() const;
    BlockStatement* body() const;
    //void set_parameters(std::vector<std::unique_ptr<Identifier>>) const;
    void append_parameters(std::unique_ptr<Identifier>);
    void set_body(std::unique_ptr<BlockStatement>);
    
  private:
    std::vector<std::unique_ptr<Identifier>> _parameters;
    std::unique_ptr<BlockStatement> _body;
  };

  class IfExpression : public Expression {
  public:
    using Expression::Expression;

    std::string to_string() const override;
    Expression* condition() const;
    BlockStatement* consequence() const;
    BlockStatement* alternative() const;
    void set_condition(std::unique_ptr<Expression>);
    void set_consequence(std::unique_ptr<BlockStatement>);
    void set_alternative(std::unique_ptr<BlockStatement>);
    
  private:
    std::unique_ptr<Expression> _condition;
    std::unique_ptr<BlockStatement> _consequence;
    std::unique_ptr<BlockStatement> _alternative;
  };

  class CallExpression : public Expression {
  public:
    using Expression::Expression;

    std::string to_string() const override;
    Expression* function() const; 
    std::vector<std::unique_ptr<Expression>> const& arguments() const;
    void set_function(std::unique_ptr<Expression>);
    void append_arguments(std::unique_ptr<Expression>); 
    
  private:
    std::unique_ptr<Expression> _function; 
    std::vector<std::unique_ptr<Expression>> _arguments;
  };

  class IndexExpression : public Expression {
  public:
    using Expression::Expression;

    std::string to_string() const override;
    Expression* index() const;
    Expression* array() const;
    void set_index(std::unique_ptr<Expression>);
    void set_array(std::unique_ptr<Expression>);

  private:
    std::unique_ptr<Expression> _index;
    std::unique_ptr<Expression> _array;
  };
  
  class PrefixExpression : public Expression {
  public:
    PrefixExpression(Token);

    std::string to_string() const override;
    std::string op() const;
    Expression* right() const;
    void set_right(std::unique_ptr<Expression>); 
    
  private:
    std::string _op; 
    std::unique_ptr<Expression> _right; 
  };

  class InfixExpression : public Expression {
  public:
    InfixExpression(Token);

    std::string to_string() const override;
    std::string op() const;
    Expression* left() const;
    Expression* right() const;
    void set_right(std::unique_ptr<Expression>);
    void set_left(std::unique_ptr<Expression>);
    
  private:
    std::unique_ptr<Expression> _left;
    std::string _op;
    std::unique_ptr<Expression> _right;
  };
}
