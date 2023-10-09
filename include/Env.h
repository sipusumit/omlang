#pragma once

#include <map>
#include <memory>
#include <llvm/IR/Value.h>

typedef struct {
  llvm::Value* value;
  llvm::Type* type;
} var_t;

class Env{
private:
  std::map<std::string, var_t> variables;
  std::shared_ptr<Env> parent;
  std::string_view src, fileName;
public:
  Env(std::string_view src, std::string_view fileName): src(src), fileName(fileName){parent = nullptr;};
  Env(std::shared_ptr<Env> parent): parent(parent){};
  bool hasVar(std::string name);
  void addVar(std::string name, var_t value);
  var_t getVal(std::string name);
  std::string_view getSrc();
  std::string_view getFilename();
};