#ifndef ULTRA_RUBY_TOKENQUEUE_H
#define ULTRA_RUBY_TOKENQUEUE_H

#include <memory>
#include <utility>
#include <vector>
#include "Token.h"

namespace UltraRuby {
namespace Lexer {
class Lexer;

class TokenQueue {
public:
    explicit TokenQueue(Lexer *lexer) : lexer(lexer), ptr(-1) {}

    const std::string &getStr() const {
        return tokens[ptr].identifier;
    }

    AST::OperationType getOperation() const {
        return tokens[ptr].operation;
    }

    TokenType getNextToken();

    void removeLast() {
        tokens.erase(tokens.end()--);
    }

    void pushBack(TokenType type) {
        tokens.emplace_back(type, AST::OP_UNKNOWN, std::string());
    }

    void pushBack(TokenType type, AST::OperationType opType) {
        tokens.emplace_back(type, opType, std::string());
    }

    void pushBack(TokenType type, const std::string &val) {
        tokens.emplace_back(type, AST::OP_UNKNOWN, val);
    }

    void pushBack(TokenType type, std::string &&val) {
        tokens.emplace_back(type, AST::OP_UNKNOWN, val);
    }

    TokenType getPrevToken() {
        if (ptr > 0) {
            ptr--;
        } else {
            //todo throw error
        }
        return tokens[ptr].type;
    }

    int getPtr() const {
        return ptr;
    }

    void setPtr(int ptr) {
        this->ptr = ptr;
    }

private:
    Lexer *lexer;
    std::vector<Token> tokens;
    int ptr;
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_TOKENQUEUE_H
