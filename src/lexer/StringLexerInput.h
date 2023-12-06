#ifndef ULTRA_RUBY_STRINGLEXERINPUT_H
#define ULTRA_RUBY_STRINGLEXERINPUT_H

#include <string>
#include <utility>

namespace UltraRuby {
namespace Lexer {

class StringLexerInput {
public:
    StringLexerInput(std::string input) : input(std::move(input)), pos(0) {}

    int getNextChar();

    bool eof();

    const std::string &getInput() const {
        return input;
    }

    int getPos() const {
        return pos;
    }

private:
    int pos;
    std::string input;
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_LEXER_INPUt_STRINGLEXERINPUT_H
