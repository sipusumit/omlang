#pragma once

#include<fstream>
#include<format>

#include "config.h"
#include "parser.h"
#include "utils.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
// #include <llvm-c/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>

#include <memory>

class Driver
{
private:
  ZConfig config;
  Parser parser;
  std::string src;
public:
  Driver(ZConfig config);
  void compile();
};
