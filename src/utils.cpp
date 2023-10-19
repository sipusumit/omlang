#include "utils.h"

std::vector<std::string> split(const std::string &s, char delim){
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;
  while(std::getline(ss, item, delim)){
    result.push_back(item);
  }
  return result;
}

std::vector<std::string> split(std::string_view s, char delim){
  return split(std::string(s), delim);
}

void unexpected(std::string_view file, std::string line, Token t){
    std::string formatedLine = line.substr(0,t.col -1) + COLOR_RED + t.value + COLOR_RESET + line.substr(t.col + t.value.size() -1);
    std::cout << file << ":" << t.line << ":" << t.col << ": ";
    std::cout << COLOR_RED "error" COLOR_RESET ": Unexpected Token: " << t.value << std::endl;
    std::cout << t.line << " |  " << formatedLine << "\n"
          << std::string(std::to_string(t.line).length(), ' ') << " | ";
    std::cout << std::string(t.col, ' ') << "^";
    if(t.value.size() > 2){
      std::cout << std::string(t.value.size() - 1, '~');
    }
}

std::string parseString(std::string str){
  if(str.size() > 2 && str[0] == '"' && str[str.size() -1 ] == '"'){
    str = str.substr(1, str.size()-2);
  }

  std::regex r(R"(\\n)");
  //TODO: replace escape characters to real escape characters
  str = std::regex_replace(str, r, "\n");
  return str;
}

std::string basename(std::string path){
  return path.substr(path.find_last_of("/\\") + 1);
}

llvm::Type *getTypefromStr(std::string_view type, llvm::IRBuilder<> *builder)
{
  if(type == "int"){
    return builder->getInt32Ty();
  }else if(type == "string"){
    return builder->getInt8PtrTy();
  }else if(type == "void"){
    return builder->getVoidTy();
  }
  return nullptr;
}

bool fileExist(std::string path)
{
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

std::string readFile(std::string path){
  std::ifstream t(path);
  std::stringstream buffer;
  buffer << t.rdbuf();
  std::string s = buffer.str();
  t.close();
  return s; 
}