#ifndef ULTRA_RUBY_CODEGENEXCEPTION_H
#define ULTRA_RUBY_CODEGENEXCEPTION_H

#include "../lexer/SourceCodeException.h"

namespace UltraRuby {
namespace IR {

class CodegenException : public Lexer::SourceCodeException {
public:
    CodegenException(const std::string &reason, int row, int col) : SourceCodeException(reason, row, col) {}
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_CODEGENEXCEPTION_H
