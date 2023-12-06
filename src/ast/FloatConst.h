#ifndef ULTRA_RUBY_AST_FLOATCONST_H
#define ULTRA_RUBY_AST_FLOATCONST_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class FloatConst : public Statement {
public:
    FloatConst(std::string val, int row, int col) : Statement(STMT_FLOAT_CONST, row, col), val(std::move(val)) {}

    const std::string &getVal() const {
        return val;
    }

private:
    std::string val;
};

}
}

#endif //ULTRA_RUBY_AST_FLOATCONST_H
