#include "FileLexerInput.h"

namespace UltraRuby {
namespace Lexer {
int FileLexerInput::getNextChar() {
    int c = ifstream.get();
    if (ifstream.eof()) {
        return 0;
    }
    return c;
}

bool FileLexerInput::eof() {
    return ifstream.eof();
}
} // UltraRuby
} // Lexer