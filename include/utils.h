#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include<fstream>
#include <sys/stat.h>
#include<algorithm>

#include<llvm/IR/Type.h>
#include<llvm/IR/IRBuilder.h>

#include "lexer.h"

#define COLOR_RED "\033[31m"

#define COLOR_RESET "\033[0m"

std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> split(std::string_view s, char delim);

void unexpected(std::string_view file, std::string line, Token t);

std::string parseString(std::string str);
std::string basename(std::string path);
std::string readFile(std::string path);

llvm::Type* getTypefromStr(std::string_view type, llvm::IRBuilder<> *builder);

bool fileExist(std::string path);


class Modules{
private:
  std::vector<std::string> list;
  static Modules *instancePtr;
  Modules(){};
public:
  Modules(const Modules& obj)= delete;
  static Modules* getInstance(){
    if(instancePtr == nullptr){
      instancePtr = new Modules();
      return instancePtr;
    }else{
      return instancePtr;
    }
  }
  static bool has(std::string module){
    return std::find(getInstance()->list.begin(), getInstance()->list.end(), module) != getInstance()->list.end();
  }

  static void add(std::string module){
    getInstance()->list.push_back(module);
  }
};