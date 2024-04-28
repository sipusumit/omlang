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
  if(config.compileOnly){
    std::string tmpLLFile = basename(config.inputFile) + ".ll";
    std::error_code EC;
    llvm::raw_fd_ostream OS(tmpLLFile, EC);
    if(EC){
      std::cout << "Error creating output stream";
    }
    std::cout << "Writing to file " << tmpLLFile << "\n";
    module->print(OS,nullptr);
    OS.flush();
    return;
  }
  // ===============================================================

  // GENERATE OBJECT FILE ==========================================
  auto TargetTriple = llvm::sys::getDefaultTargetTriple();
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
  if(!Target){
    llvm::errs() << Error;
    exit(1);
  }

  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opt;
  auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);
  module->setDataLayout(TargetMachine->createDataLayout());
  module->setTargetTriple(TargetTriple);

  std::error_code EC2;
  llvm::raw_fd_ostream dest(basename(config.inputFile) + ".o", EC2, llvm::sys::fs::OF_None);

  if (EC2) {
    llvm::errs() << "Could not open file: " << EC2.message();
    exit(1);
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CodeGenFileType::CGFT_ObjectFile;

  if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TargetMachine can't emit a file of this type";
    exit(1);
  }

  pass.run(*module);
  dest.flush();
  //================================================================

  // Compile to executable ==================================================
  // std::cout << "Compiling " << tmpLLFile << " to a." O_EXT << "\n";
  // system(std::format("clang -o {} {} {}/../lib/libstdom.a", config.outputFile, tmpLLFile, config.exePath).c_str());
  // ========================================================================
}