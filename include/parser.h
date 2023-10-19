#pragma once

#include<format>

#include "lexer.h"
#include "AST.h"
#include"config.h"

class Parser
{
private:
  std::string file, src;
  Lexer lexer;
  std::vector<Token> tokens;
  std::vector<std::shared_ptr<AST>> stmts;

  Token eat();
  Token eat(TType type);
  Token eat(TType type, std::string_view msg);
  void error(Token t);
  void error(Token t, std::string_view msg);

  std::shared_ptr<AST> parse_stmt(bool parseSColon);
  std::shared_ptr<FunctionDeclAST> function_decl();
  std::shared_ptr<FunctionDefAST> function_def();
  std::shared_ptr<VarDeclAST> var_decl();
  std::shared_ptr<AST> parse_ident_or_fcall();
  std::shared_ptr<FCall> parse_fcall(Token name);
  std::shared_ptr<ReturnAST> parse_return();
  std::shared_ptr<StringAST> parse_string();
  std::shared_ptr<IntegerAST> parse_int();
  std::shared_ptr<AST> parse_use();
public:
  Parser(){};
  Parser(std::string file, std::string src): file(file), src(src){
    lexer = Lexer(file, src);

    Token t = lexer.getNextToken();
    while(t.type != TType::TEOF){
      if(t.type == TType::WS || t.type == TType::NL){
        t = lexer.getNextToken();
        continue;
      }
      tokens.push_back(t);
      t = lexer.getNextToken();
    }
    tokens.push_back(t);
  };
  std::vector<std::shared_ptr<AST>> parse();
};