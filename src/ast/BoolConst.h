#ifndef ULTRA_RUBY_AST_BOOLCONST_H
#define ULTRA_RUBY_AST_BOOLCONST_H

#include "Statement.h"

namespace UltraRuby {
namespace AST {

class BoolConst : public Statement {
public:
    BoolConst(bool val, int row, int col) : Statement(STMT_BOOL_CONST, row, col), val(val) {}

    bool getVal() const {
        return val;
    }

private:
    bool val;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_BOOLCONST_H
