#ifndef ULTRA_RUBY_LEXER_LEXERINPUT_H
#define ULTRA_RUBY_LEXER_LEXERINPUT_H

namespace UltraRuby::Lexer {

class LexerInput {
public:
    virtual ~LexerInput() = default;

    virtual int getNextChar() = 0;
};

}

#endif //ULTRA_RUBY_LEXER_LEXERINPUT_H
