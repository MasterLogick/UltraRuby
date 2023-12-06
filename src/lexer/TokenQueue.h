#ifndef ULTRA_RUBY_LEXER_TOKENQUEUE_H
#define ULTRA_RUBY_LEXER_TOKENQUEUE_H

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

    int getRow() const {
        return tokens[ptr].row;
    }

    int getCol() const {
        return tokens[ptr].col;
    }

    TokenType getNextToken();

    void removeLast() {
        tokens.erase(tokens.end()--);
    }

    void pushBack(TokenType type, int row, int col) {
        tokens.emplace_back(type, AST::OP_UNKNOWN, std::string(), row, col);
    }

    void pushBack(TokenType type, AST::OperationType opType, int row, int col) {
        tokens.emplace_back(type, opType, std::string(), row, col);
    }

    void pushBack(TokenType type, const std::string &val, int row, int col) {
        tokens.emplace_back(type, AST::OP_UNKNOWN, val, row, col);
    }

    void pushBack(TokenType type, std::string &&val, int row, int col) {
        tokens.emplace_back(type, AST::OP_UNKNOWN, val, row, col);
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

#endif //ULTRA_RUBY_LEXER_TOKENQUEUE_H
