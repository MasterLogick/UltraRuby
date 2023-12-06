#ifndef ULTRA_RUBY_LEXER_LEXER_H
#define ULTRA_RUBY_LEXER_LEXER_H

#include <memory>
#include <utility>
#include "StringLexerInput.h"
#include "TokenType.h"
#include "../ast/OperationType.h"
#include "TokenQueue.h"
#include "StringLexerInput.h"

namespace UltraRuby::Lexer {
class Lexer {
public:
    explicit Lexer(std::shared_ptr<StringLexerInput> input);

    void emmitToken();

    TokenQueue *getQueue() {
        return &queue;
    }

private:
    std::shared_ptr<StringLexerInput> input;
    TokenQueue queue;
    std::vector<int> lineBeginnings;
    int lastChar;
    int row;
    bool eofReached;
};

}

#endif //ULTRA_RUBY_LEXER_LEXER_H
