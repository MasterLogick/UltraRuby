#ifndef ULTRA_RUBY_BOOLCONST_H
#define ULTRA_RUBY_BOOLCONST_H

#include "Statement.h"

namespace UltraRuby {
namespace AST {

class BoolConst : public Statement {
public:
    BoolConst(bool val) : Statement(StatementType::STMT_BOOL_CONST), val(val) {}

private:
    bool val;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_BOOLCONST_H
