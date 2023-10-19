#include "parser.h"

template <typename Enumeration>
auto as_integer(Enumeration const value)-> typename std::underlying_type<Enumeration>::type{
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

Token Parser::eat(){
  Token t = tokens[0];
  tokens.erase(tokens.begin());
  return t;
}


Token Parser::eat(TType type){
  Token t = eat();
  if(t.type != type){
    error(t);
  }
  return t;
}

Token Parser::eat(TType type, std::string_view msg){
  Token t = eat();
  if(t.type != type){
    error(t, msg);
  }
  return t;
}

void Parser::error(Token t, std::string_view msg){
  unexpected(file, split(src, '\n')[t.line -1], t);
  std::cout << "\n" << msg;
  exit(1);
}

void Parser::error(Token t){
  error(t, "");
}

std::vector<std::shared_ptr<AST>> Parser::parse(){
  while(tokens[0].type != TType::TEOF){
    stmts.push_back(std::move(parse_stmt(true)));
  }
  return stmts;
}

std::shared_ptr<AST> Parser::parse_stmt(bool parseSColon){
  std::shared_ptr<AST> a;
  switch (tokens[0].type){
    case TType::USE:
      a = parse_use();
      if(parseSColon) eat(TType::SCOLON).type == TType::SCOLON;
      break;
    case TType::FUN:
    case TType::EXTERN:
      a = function_def();
      break;
    case TType::LET:
    case TType::CONST:
      a =  var_decl();
      if(parseSColon) eat(TType::SCOLON).type == TType::SCOLON;
      break;
    case TType::IDENTIFIER:
      a =  parse_ident_or_fcall();
      if(parseSColon) eat(TType::SCOLON).type == TType::SCOLON;
      break;
    case TType::RETURN:
      a =  parse_return();
      if(parseSColon) eat(TType::SCOLON).type == TType::SCOLON;
      break;
    case TType::STRING:
      a =  parse_string();
      if(parseSColon) eat(TType::SCOLON).type == TType::SCOLON;
      break;
    case TType::INT:
      a =  parse_int();
      if(parseSColon) eat(TType::SCOLON).type == TType::SCOLON;
      break;
    case TType::TEOF:
      std::cout << "Unexpected end of file\n";
      exit(1);
    default:
      error(tokens[0], "default");
      break;
  }
  return a;
}

std::shared_ptr<AST> Parser::parse_use(){
  eat(); // #use
  std::string header = std::format("{}/../include/{}.om", Config::getInstance()->exePath, eat().value);
  if(fileExist(header)){
    Parser p(header,readFile(header));
    std::vector<std::shared_ptr<AST>> _stmts = p.parse();
    stmts.reserve(stmts.size() + _stmts.size());
    stmts.insert(stmts.end(), _stmts.begin(), _stmts.end() - 1);
    return _stmts.at(_stmts.size()-1);
  }
  return std::make_shared<AST>();
}

// func_decl
//   : IDENTIFIER ( a : string , b: int): TYPE ;
std::shared_ptr<FunctionDeclAST> Parser::function_decl()
{
  Token name = eat(TType::IDENTIFIER);
  eat(TType::LPAREN); // (
  fifo_map<std::string, std::string> args;
  while(tokens[0].type != TType::RPAREN){
    std::string name = eat(TType::IDENTIFIER).value;
    eat(TType::COLON);
    std::string type = eat().value;
    args[name] = type;
    if(tokens[0].type != TType::COMMA && tokens[0].type != TType::RPAREN){
      error(tokens[0], "Expected a comma seperated list of arguments");
    }
    if(tokens[0].type == TType::COMMA){ // comma seperated args
      eat(TType::COMMA);
    }
  }

  eat(TType::RPAREN); // )
  eat(TType::COLON); // :
  Token type = eat(); // type
  // std::cout << type.value << "\n";
  return std::make_shared<FunctionDeclAST>(name.value, type.value, args, name);
};

// }
// fun IDENTIFIER( args ): TYPE { stmt }
std::shared_ptr<FunctionDefAST> Parser::function_def(){
  bool isExtern = false;
  if(tokens[0].type == TType::EXTERN){
    eat();
    isExtern = true;
  }else{
    eat(TType::FUN);
  }
  
  std::shared_ptr<FunctionDeclAST> proto = function_decl();
  if(isExtern){
    eat(TType::SCOLON);
    return std::make_shared<FunctionDefAST>(proto, isExtern);
  }
  eat(TType::LBRACE); // {
  std::vector<std::shared_ptr<AST>> body;
  while(tokens[0].type != TType::RBRACE){ // }
    body.push_back(std::move(parse_stmt(true)));
  }
  eat(TType::RBRACE); // }
  StatementsAST s = StatementsAST(body);
  return std::make_shared<FunctionDefAST>(proto, std::make_shared<StatementsAST>(s));
}

// let IDENTIFIER : type = stmt ;
// let IDENTIFIER : type;
std::shared_ptr<VarDeclAST> Parser::var_decl()
{
  bool  isConst = eat().value == "const";
  std::string name = eat(TType::IDENTIFIER).value;
  eat(TType::COLON, "Expected colon seperated variable type");
  Token t_type = eat();
  VType t;
  switch (t_type.type)
  {
  case TType::T_INT:
    t = VType::INT;
    break;
  case TType::T_STRING:
    t = VType::STRING;
    break;
  default:
    error(t_type,"Unknown variable type");
    break;
  }
  if(tokens[0].type == TType::SCOLON){
    // eat(TType::SCOLON);
    return std::make_shared<VarDeclAST>(t,name);
  }
  eat(TType::EQUALS); // =
  std::shared_ptr<AST> value = parse_stmt(false);
  // eat(TType::SCOLON); // ;
  return std::make_shared<VarDeclAST>(t,name,value);
}
// ident_or_fcall
//  : IDENTIFIER 
//  | IDENTIFIER ( ) ;
std::shared_ptr<AST> Parser::parse_ident_or_fcall(){
  Token t_name = eat(TType::IDENTIFIER);
  if(tokens[0].type != TType::LPAREN){
    return std::make_shared<IdentifierAST>(t_name.value, t_name);
  }
  return parse_fcall(t_name);
}

// fcall
//   : IDENTIFIER ( ) ;
//   ;
std::shared_ptr<FCall> Parser::parse_fcall(Token name){
  eat(TType::LPAREN, "Expected '('"); // (
  // ARGS
  std::vector<std::shared_ptr<AST>> args;
  while(tokens[0].type != TType::RPAREN){
    args.push_back(parse_stmt(false));
    if(tokens[0].type != TType::COMMA && tokens[0].type != TType::RPAREN){
      error(tokens[0], "Expected a comma seperated list of arguments");
    }
    if(tokens[0].type == TType::COMMA){ // comma seperated args
      eat(TType::COMMA);
    }
  }
  eat(TType::RPAREN, "Expected ')'"); // )
  // eat(TType::SCOLON, "Expected semicolon"); // ;
  return std::make_shared<FCall>(name, name.value, args);
}

std::shared_ptr<ReturnAST> Parser::parse_return(){
  eat(TType::RETURN); // return
  // Token t = eat();
  std::shared_ptr<ReturnAST> r;
  // if(t.type == TType::INT){
  //   IntegerAST v = IntegerAST(std::stoi(t.value));
  //   r = std::make_shared<ReturnAST>(std::make_shared<IntegerAST>(v));
  // }
  // else if(t.type == TType::STRING){
  //   StringAST v = StringAST(t.value);
  //   r = std::make_shared<ReturnAST>(std::make_shared<StringAST>(v));
  // }else{
  //   std::cout << "Unexpected Return Type";
  //   exit(1);
  // }
  r = std::make_shared<ReturnAST>(parse_stmt(false));
  // eat(TType::SCOLON); //;
  return r;
}

std::shared_ptr<StringAST> Parser::parse_string(){
  return std::make_shared<StringAST>(eat(TType::STRING).value);
}

std::shared_ptr<IntegerAST> Parser::parse_int(){
  return std::make_shared<IntegerAST>(eat(TType::INT).value);
}