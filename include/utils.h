#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <regex>

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

llvm::Type* getTypefromStr(std::string_view type, llvm::IRBuilder<> *builder);