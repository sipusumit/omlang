#include "driver.h"

Driver::Driver(ZConfig config){
  this->config = config;
  parser = Parser(config.inputFile , readFile(config.inputFile));
}

void setupstd(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Module *module){
  module->getOrInsertFunction("print",
                              llvm::FunctionType::get(builder->getVoidTy(),
                                builder->getInt8Ty()->getPointerTo(),
                                false
                              ));
  // module->getOrInsertFunction("scanf",
  //                             llvm::FunctionType::get(builder->getInt32Ty(),
  //                               builder->getInt8Ty()->getPointerTo(),
  //                               true
  //                             ));
}

void Driver::compile(){
  // Lexer l = Lexer(config.inputFile, std::string(src));
  // Token t = l.getNextToken();
  // while(t.type != TType::TEOF){
  //   std::cout << t.value << "\n\n";
  //   t = l.getNextToken();
  // }
  // exit(0);

  std::vector<std::shared_ptr<AST>> stmts = parser.parse();

  llvm::LLVMContext context = llvm::LLVMContext();
  std::shared_ptr<llvm::Module> module = std::make_shared<llvm::Module>("test", context);
  module->setSourceFileName(config.inputFile);
  module->setTargetTriple(LLVMGetDefaultTargetTriple());
  llvm::IRBuilder<> builder(context);

  std::shared_ptr<Env> env = std::make_shared<Env>(src,config.inputFile);
  // setup standard functions
  // setupstd(&context, &builder, module.get());

  for(auto const& s : stmts){
    s->codegen(&context, &builder, module.get(), env);
  }

  // PRINT TO TERMINAL
  // if(config.outputFile == "a." O_EXT){
  //   module->print(llvm::outs(), nullptr);
  //   llvm::outs().flush();
  // }

  // SAVE IR TO FILE ===============================================
  std::string tmpLLFile = basename(config.inputFile) + ".ll";
  std::error_code EC;
  llvm::raw_fd_ostream OS(tmpLLFile, EC);
  if(EC){
    std::cout << "Error creating output stream";
  }
  std::cout << "Writing to file " << tmpLLFile << "\n";
  module->print(OS,nullptr);
  OS.flush();
  // ===============================================================

  // Compile to executable ==================================================
  std::cout << "Compiling " << tmpLLFile << " to a." O_EXT << "\n";
  system(std::format("clang -o {} {} {}/../lib/libstdom.a", config.outputFile, tmpLLFile, config.exePath).c_str());
  // ========================================================================
}