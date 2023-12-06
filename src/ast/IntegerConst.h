#ifndef ULTRA_RUBY_AST_INTEGERCONST_H
#define ULTRA_RUBY_AST_INTEGERCONST_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class IntegerConst : public Statement {
public:
    IntegerConst(std::string val, int row, int col) : Statement(STMT_INT_CONST, row, col), val(std::move(val)) {}

    const std::string &getVal() const {
        return val;
    }

private:
    std::string val;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_INTEGERCONST_H
