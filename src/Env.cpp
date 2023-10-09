#include "Env.h"

bool Env::hasVar(std::string name)
{
  if(variables.count(name))
    return true;
  if(parent != nullptr)
    return parent->hasVar(name);
  return false;
}

void Env::addVar(std::string name, var_t value)
{
  variables[name] = value;
}

var_t Env::getVal(std::string name)
{
  if(hasVar(name))
    return variables[name];
  
  if(parent != nullptr)
    return parent->getVal(name);
  
  return {};
}

std::string_view Env::getSrc(){
  if(parent == nullptr)
    return src;
  return parent->getSrc();
}

std::string_view Env::getFilename()
{
  if(parent == nullptr)
    return fileName;
  return parent->getFilename();
}
