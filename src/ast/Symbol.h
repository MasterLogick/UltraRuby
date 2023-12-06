#ifndef ULTRA_RUBY_AST_SYMBOL_H
#define ULTRA_RUBY_AST_SYMBOL_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Symbol : public Statement {
public:
    Symbol(std::string sym, int row, int col) : Statement(STMT_SYMBOL, row, col), sym(std::move(sym)) {}

    const std::string &getSym() const {
        return sym;
    }

private:
    std::string sym;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_SYMBOL_H
