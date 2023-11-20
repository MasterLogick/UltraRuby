#ifndef ULTRA_RUBY_PARSEREWINDGUARD_H
#define ULTRA_RUBY_PARSEREWINDGUARD_H

#include "Parser.h"

namespace UltraRuby {
namespace Parser {

class ParseRewindGuard {
public:
    ParseRewindGuard(Parser *parser);

    ~ParseRewindGuard();

    void release();
private:
    int rewind;
    Parser *parser;
};

} // UltraRuby
} // Parser

#endif //ULTRA_RUBY_PARSEREWINDGUARD_H
