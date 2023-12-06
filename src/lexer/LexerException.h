#ifndef ULTRA_RUBY_LEXEREXCEPTION_H
#define ULTRA_RUBY_LEXEREXCEPTION_H

#include "SourceCodeException.h"

namespace UltraRuby {
namespace Lexer {

class LexerException : public SourceCodeException {
public:
    LexerException(const std::string &reason, int row, int col) : SourceCodeException(reason, row, col) {}
};

} // UltraRuby
} // Lexer

#endif //ULTRA_RUBY_LEXEREXCEPTION_H
