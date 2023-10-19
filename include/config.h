#pragma once
#include <string>

#ifndef O_EXT
#define O_EXT "out"
#endif

class ZConfig
{
public:
  bool compileOnly;
  std::string outputFile, inputFile, exePath;


  ZConfig(){
    this->compileOnly = false;
    this->outputFile = "a." O_EXT;
  };
  ZConfig(bool compileOnly, std::string outputFile, std::string inputFile, std::string exePath): compileOnly(compileOnly), outputFile(outputFile), inputFile(inputFile), exePath(exePath){};
};


class Config{
private:
  static Config *instancePtr;
  Config(){};
public:
  bool compileOnly;
  std::string outputFile, inputFile, exePath;
  Config(const Config& obj)= delete;
  static Config* getInstance(){
    if(instancePtr == nullptr){
      instancePtr = new Config();
      return instancePtr;
    }else{
      return instancePtr;
    }
  }
};