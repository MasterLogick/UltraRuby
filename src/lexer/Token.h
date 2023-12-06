#ifndef ULTRA_RUBY_LEXER_TOKEN_H
#define ULTRA_RUBY_LEXER_TOKEN_H

#include <string>
#include "TokenType.h"
#include "../ast/OperationType.h"

namespace UltraRuby {
namespace Lexer {

struct Token {
    TokenType type = TOK_ERROR;
    AST::OperationType operation = AST::OP_UNKNOWN;
    std::string identifier;
    int row;
    int col;
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_LEXER_TOKEN_H
