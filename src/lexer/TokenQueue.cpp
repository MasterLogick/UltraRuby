#include "TokenQueue.h"
#include "Lexer.h"

namespace UltraRuby {
namespace Lexer {
TokenType TokenQueue::getNextToken() {
    ptr++;
    if (ptr == tokens.size()) {
        lexer->emmitToken();
    }
    return tokens[ptr].type;
}
}
}