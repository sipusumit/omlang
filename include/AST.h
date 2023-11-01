#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LegacyPassManager.h>

#include "lexer.h"
#include "Env.h"
#include "fifo_map.hpp"

using namespace nlohmann;

class AST{
protected:
  Token token;
public:
  AST(){token = Token{};};
  AST(Token t): token(t){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
    // std::cout << "BASE \n";
    return nullptr;
  };
};

class IntegerAST : public AST{
private:
  int value;
public:
  IntegerAST(int value): value(value){};
  IntegerAST(std::string value): value(std::stoi(value)){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class StringAST : public AST{
private:
  std::string value;
public:
  StringAST(std::string value): value(value){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class StatementsAST : public AST{
private:
  std::vector<std::shared_ptr<AST>> stmts;
public:
  StatementsAST(std::vector<std::shared_ptr<AST>> stmts): stmts(std::move(stmts)){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class FunctionDeclAST{
private:
  std::string name, type;
  fifo_map<std::string,std::string> arguments;
  Token t;
public:
  FunctionDeclAST(){};
  FunctionDeclAST(std::string name, std::string type, fifo_map<std::string,std::string> arguments, Token t): name(name), type(type), arguments(arguments), t(t){};
  std::string getName();
  llvm::Function *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env);
};

class FunctionDefAST : public AST{
private:
  std::shared_ptr<FunctionDeclAST> proto;
  std::shared_ptr<StatementsAST> body;
  bool isExtern;
public:
  FunctionDefAST(std::shared_ptr<FunctionDeclAST> proto, std::shared_ptr<StatementsAST> body): proto(proto), body(body), isExtern(false){};
  FunctionDefAST(std::shared_ptr<FunctionDeclAST> proto, bool isExtern): proto(proto), isExtern(isExtern){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class FCall : public AST{
private:
  std::string name;
  std::vector<std::shared_ptr<AST>> args;
public:
  FCall(Token t, std::string name): name(name), AST(t){};
  FCall(Token t, std::string name, std::vector<std::shared_ptr<AST>> args): name(name), args(args), AST(t){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class ReturnAST : public AST{
private:
  std::shared_ptr<AST> value;
public:
  ReturnAST(){value = std::make_shared<IntegerAST>(0);};
  ReturnAST(std::shared_ptr<AST> value): value(std::move(value)){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class VarDeclAST : public AST{
private:
  VType type;
  std::string name;
  std::shared_ptr<AST> value;
public:
  VarDeclAST(VType type, std::string name, std::shared_ptr<AST> value): type(type), name(name), value(value){};
  VarDeclAST(VType type, std::string name): type(type), name(name), value(nullptr){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class IdentifierAST : public AST{
private:
  std::string name;
public:
  IdentifierAST(std::string name, Token t): name(name), AST(t){};
  std::string getName(){return name;};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};

class BinOPAST : public AST{
private:
  std::shared_ptr<AST> lhs, rhs;
  char op;
public:
  BinOPAST(std::shared_ptr<AST> lhs, char op, std::shared_ptr<AST> rhs): lhs(lhs), op(op), rhs(rhs){};
  virtual llvm::Value *codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env) override;
};