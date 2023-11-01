#include <iostream>
#include <chrono>
// #include <boost/program_options.hpp>

#include "driver.h"
#include "cxxopts.hpp"


#ifndef Z_VERSION
#define Z_VERSION "unknown"
#endif


#ifdef WIN32
  #include "libloaderapi.h"
#endif
/*
  Usage
    om [options] file
  Options
    --help              Display this Menu
    --version           Display Version Information
    -c                  Output an object file
    -S                  Output an llvm ir file
    -o <file>           Output to <file>

*/

using namespace std::chrono;

Config* Config::instancePtr = nullptr;
Modules* Modules::instancePtr = nullptr;

int main(int argc, char *argv[]){
  // CLI Arguments
  cxxopts::Options arguments(basename(argv[0]), "A Toy Compiler Built using LLVM");
  arguments.add_options()
  ("h,help", "Display this Menu")
  ("v,version", "Display Version")
  ("c", "Output an object file")
  ("S", "Output an llvm ir file")
  ("o", "Output to <file>", cxxopts::value<std::string>()->default_value("a." O_EXT), "<file>")
  ;
  // to get input file without positional arguments
  arguments.allow_unrecognised_options();
  if(argc < 2){
    std::cout << arguments.help({}, true);
    exit(0);
  }

  cxxopts::ParseResult result = arguments.parse(argc, argv);
  
  if(result.count("help")){
    std::cout << arguments.help({}, true);
    return 0;
  }else if(result.count("version")){
    std::cout << "v" VERSION;
    return 0;
  }
  // Config for compiler
  ZConfig conf;
  conf.compileOnly = result.count("c") ? true : false;
  Config::getInstance()->compileOnly = conf.compileOnly;
  conf.outputFile = result.count("o") ? result["o"].as<std::string>() : "a." O_EXT;
  Config::getInstance()->outputFile = conf.outputFile;
  
  cxxopts::OptionNames unmatched = result.unmatched();
  conf.inputFile = unmatched.size() > 0 ? unmatched[0] : nullptr;
  #ifdef WIN32
    // LPSTR filePath[MAX_PATH];
    // GetModuleFileNameA(NULL, *filePath, MAX_PATH);
    char filePath[MAX_PATH];
    GetModuleFileName(NULL, filePath, MAX_PATH);
    std::string f = std::string(filePath);
    conf.exePath = f.substr(0, f.find_last_of("/\\"));
    Config::getInstance()->exePath = conf.exePath;
    // std::cout << conf.exePath;
    // conf.exePath = "D:/projects/omlang/build/bin";
    // Config::getInstance()->exePath = conf.exePath;

  #endif
  auto start = high_resolution_clock::now();
  Driver driver(conf);
  driver.compile();
  auto end = high_resolution_clock::now();

  std::cout << "\nTime Taken: " << duration_cast<milliseconds>(end -start).count() << "ms";

  return 0;
}
