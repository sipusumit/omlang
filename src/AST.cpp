#include "AST.h"

llvm::Value *IntegerAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
  return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), value);
}

llvm::Value *StringAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
  return builder->CreateGlobalStringPtr(value);
}

llvm::Value *StatementsAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
  for(auto const& a : stmts){
    a->codegen(context, builder, module, env);
  }
  return nullptr;
}

llvm::Value *FCall::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
  auto fn = module->getFunction(name);
  if(!fn){
    unexpected(env->getFilename(),split(env->getSrc(), '\n')[token.line -1],token);
    std::cout << "\nCall to undefined function " << name << std::endl;
    exit(1);
  }
  if(args.size() == 0)
    return builder->CreateCall(fn);
  std::vector<llvm::Value*> arguments;
  for(auto const& arg : args){
    arguments.push_back(arg->codegen(context, builder, module, env));
  }
  return builder->CreateCall(fn,arguments);
}

llvm::Value *ReturnAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
  return builder->CreateRet(value->codegen(context, builder, module, env));
}

llvm::Value *VarDeclAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env)
{
  llvm::Type *ty;
  switch (type)
  {
  case VType::INT:
    ty = builder->getInt32Ty();
    break;
  case VType::STRING:
    ty = builder->getInt8PtrTy();
    break;
  default:
    exit(1);
    break;
  }
  llvm::Value* v = builder->CreateAlloca(ty);
  env->addVar(name,{.value = v, .type = ty});
  if(value == nullptr)
    return v;
  llvm::Value* vl = value->codegen(context, builder, module, env);
  return builder->CreateStore(vl,v);
}

llvm::Value *IdentifierAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env)
{
  var_t value = env->getVal(name);
  if(value.value == nullptr){
    unexpected(env->getFilename(),split(env->getSrc(), '\n')[token.line -1],token);
    std::cout << "\nIdentifier " << name << " was not defined in this scope" << std::endl;
    exit(1);
  }
  //TODO: add by address support
  // return value.value;
  return builder->CreateLoad(value.type,value.value,name);
}

llvm::Value *BinOPAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env)
{
  return nullptr;
}


std::string FunctionDeclAST::getName(){
  return name;
}

llvm::Function *FunctionDeclAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env)
{
  llvm::Function *f = module->getFunction(name);
  if(f != nullptr){
    unexpected(env->getFilename(),split(env->getSrc(),'\n')[t.line -1], t);
    std::cout << "\nFunction already Declared " << name;
    exit(1);
  }

  std::vector<llvm::Type *> argsTY;

  for(auto a : arguments){
    llvm::Type *ty = getTypefromStr(a.second, builder);
    // env->addVar(std::string(a.first), {.type = ty});
    argsTY.push_back(ty);
  }

  llvm::FunctionType *ft = llvm::FunctionType::get(getTypefromStr(type, builder), argsTY, false);
  f = llvm::Function::Create(ft,llvm::Function::ExternalLinkage,name,module);
  fifo_map<std::string,std::string>::iterator idx = arguments.begin();
  for(auto &arg : f->args()){
    arg.setName(idx->first);
    // env->addVar(idx->first, var_t{.value = &arg, .type=arg.getType()});

  }
  return f;
}

llvm::Value *FunctionDefAST::codegen(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module, std::shared_ptr<Env> env){
  // new scope
  std::shared_ptr<Env> nEnv(env);
  // llvm::Function *f = module->getFunction(name);
  // if(f != nullptr){
  //   std::cout << "Function already Defined " << name;
  //   exit(1);
  // }

  // llvm::FunctionType *functionTyInt = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context),false);
  // f = llvm::Function::Create(functionTyInt,llvm::Function::ExternalLinkage,name,module);
  llvm::Function *f = proto->codegen(context, builder, module, nEnv);
  if(isExtern){
    return f;
  }
  llvm::BasicBlock *bb =  llvm::BasicBlock::Create(*context, "entry", f);
  builder->SetInsertPoint(bb);
  for(auto &arg : f->args()){
    llvm::Value* v = builder->CreateAlloca(arg.getType());
    builder->CreateStore(&arg,v);
    nEnv->addVar(arg.getName().str(), var_t{.value = v, .type=arg.getType()});
  }
  body->codegen(context, builder, module, nEnv);
  return f;
}
