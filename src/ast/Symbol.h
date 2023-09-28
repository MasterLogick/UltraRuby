#ifndef ULTRA_RUBY_SYMBOL_H
#define ULTRA_RUBY_SYMBOL_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Symbol : public Statement {
public:
    Symbol(std::string sym) : Statement(STMT_SYMBOL), sym(std::move(sym)) {}

    inline const std::string &getSym() const {
        return sym;
    }

private:
    std::string sym;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_SYMBOL_H
