#ifndef ULTRA_RUBY_PARSEEXCEPTION_H
#define ULTRA_RUBY_PARSEEXCEPTION_H

#include "../lexer/SourceCodeException.h"

namespace UltraRuby {
namespace Parser {

class ParseException : public Lexer::SourceCodeException {
public:
    ParseException(const std::string &reason, int row, int col) : SourceCodeException(reason, row, col) {}
};

} // UltraRuby
} // Parser

#endif //ULTRA_RUBY_PARSEEXCEPTION_H
