#include "StringLexerInput.h"

namespace UltraRuby {
namespace Lexer {
int StringLexerInput::getNextChar() {
    if (pos == input.size()) {
        return 0;
    }
    return input[pos++];
}

bool StringLexerInput::eof() {
    return pos == input.size();
}
} // UltraRuby
} // Lexer