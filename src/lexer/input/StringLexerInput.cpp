#include "StringLexerInput.h"

namespace UltraRuby {
namespace Lexer {
int StringLexerInput::getNextChar() {
    if (pos == input.size()) {
        return EOF;
    }
    return input[pos++];
}
} // UltraRuby
} // Lexer