#ifndef ULTRA_RUBY_LEXER_H
#define ULTRA_RUBY_LEXER_H

#include <memory>
#include <utility>
#include "LexerInput.h"
#include "TokenType.h"
#include "../ast/OperationType.h"
#include "TokenQueue.h"

namespace UltraRuby::Lexer {
class Lexer {
public:
    explicit Lexer(std::shared_ptr<LexerInput> input) : input(std::move(input)), lastChar(' '), queue(this) {
        emmitToken();
        queue.removeLast();
    }

    void emmitToken();

    inline TokenQueue *getQueue() {
        return &queue;
    }

private:
    static void logError(const char *log);

    TokenQueue queue;
    std::shared_ptr<LexerInput> input;
    int lastChar;
};

}

#endif //ULTRA_RUBY_LEXER_H
