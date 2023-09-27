#include "FileLexerInput.h"

namespace UltraRuby {
namespace Lexer {
int FileLexerInput::getNextChar() {
    int c = ifstream.get();
    if (c == std::ifstream::traits_type::eof()) {
        return EOF;
    }
    return c;
}
} // UltraRuby
} // Lexer