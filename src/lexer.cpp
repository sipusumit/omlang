#include "lexer.h"

fifo_map<std::string,TType> PATTERNS = {
    {R"(^\r?\n)", TType::NL},
    {R"(^\s+)", TType::WS},
    {R"(^\/\*([\S\s]*?)\*\/)", TType::WS},

    {R"(^\d+)", TType::INT},
    {R"(^\"(?:\\.|[^\"])*\")", TType::STRING},

    {R"(^int(?=[^a-zAZ0-9_]))", TType::T_INT},
    {R"(^string(?=[^a-zAZ0-9_]))", TType::T_STRING},
    {R"(^void(?=[^a-zAZ0-9_]))", TType::T_VOID},
    {R"(^return(?=[^a-zAZ0-9_]))", TType::RETURN},
    {R"(^\#use(?=[^a-zAZ0-9_]))", TType::USE},
    {R"(^fun(?=[^a-zAZ0-9_]))", TType::FUN},
    {R"(^let(?=[^a-zAZ0-9_]))", TType::LET},
    {R"(^const(?=[^a-zAZ0-9_]))", TType::CONST},
    {R"(^extern(?=[^a-zAZ0-9_]))", TType::EXTERN},

    {R"(^[a-zA-Z_]+[a-zA-Z0-9_]*)", TType::IDENTIFIER},

    {R"(^\;)", TType::SCOLON},
    {R"(^\:)", TType::COLON},
    {R"(^\,)", TType::COMMA},
    {R"(^\()", TType::LPAREN},
    {R"(^\))", TType::RPAREN},
    {R"(^\{)", TType::LBRACE},
    {R"(^\})", TType::RBRACE},
    {R"(^\=)", TType::EQUALS},
    {R"(^\&)", TType::AMPERSAND},
};


void Lexer::error(std::string s){
  std::cout << file << ":" << line << ":" << col << ": Error: Unknown Token\n";
  std::cout << line << " |  " << s << "\n"
            << std::string(std::to_string(line).length(), ' ') << " | ";
  std::cout << std::string(col, ' ') << "^";
  exit(1);
}

Token Lexer::getNextToken(){
  if(at >= src.length())
    return Token{.type = TType::TEOF};
  
  std::string str = src.substr(at); 
  for(fifo_map<std::string, TType>::iterator it = PATTERNS.begin(); it != PATTERNS.end(); it++){
        std::regex r(it->first);
        std::smatch m;
        regex_search(str, m, r);
        if(!m.empty()){
            at += m[0].length();
            Token t;
            if(it->second == TType::STRING){
              t = Token{.type = it->second, .value = parseString(m[0].str()), .line = line, .col = col};
            }
            else{
              t = Token{.type = it->second, .value = m[0], .line = line, .col = col};
            }
            col += m[0].length();
            if(it->second == TType::NL){
              line+=1;
              col = 1;
            }
            return t;
        }
  }
  std::string errStr = split(src, '\n')[line - 1];
  error(errStr);
  exit(1);
}