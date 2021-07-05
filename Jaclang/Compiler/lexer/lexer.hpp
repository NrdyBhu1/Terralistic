//
//  lexer.hpp
//  Jaclang
//
//  Created by Jakob Zorz on 04/07/2021.
//

#ifndef lexer_hpp
#define lexer_hpp

#include <vector>
#include <string>
#include <fstream>

enum class TokenType {SYMBOL, KEYWORD, INDENT, STRING, CONSTANT_INTEGER};
enum class Symbol {NONE, ASSIGNMENT};
enum class Keyword {NONE, IF};
 
struct Token {
    TokenType type;
    std::string text;
    Symbol symbol;
    Keyword keyword;
    int const_int;
};

std::vector<Token> tokenize(std::filebuf* file_buffer);

#endif /* lexer_hpp */