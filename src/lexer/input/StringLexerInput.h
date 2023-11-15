#ifndef ULTRA_RUBY_LEXER_INPUt_STRINGLEXERINPUT_H
#define ULTRA_RUBY_LEXER_INPUt_STRINGLEXERINPUT_H

#include <string>
#include "../LexerInput.h"

namespace UltraRuby {
namespace Lexer {

class StringLexerInput : public LexerInput {
public:
    StringLexerInput(const std::string &input) : input(input), pos(0) {}

    int getNextChar() override;

    bool eof() override;

private:
    int pos;
    std::string input;
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_LEXER_INPUt_STRINGLEXERINPUT_H
