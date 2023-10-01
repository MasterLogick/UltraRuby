#ifndef ULTRA_RUBY_AST_INTEGERCONST_H
#define ULTRA_RUBY_AST_INTEGERCONST_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class IntegerConst : public Statement {
public:
    explicit IntegerConst(std::string val) : Statement(STMT_INT_CONST), val(std::move(val)) {}

    inline const std::string &getVal() const {
        return val;
    }

private:
    std::string val;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_INTEGERCONST_H
