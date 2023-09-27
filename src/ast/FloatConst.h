#ifndef ULTRA_RUBY_FLOATCONST_H
#define ULTRA_RUBY_FLOATCONST_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class FloatConst : public Statement {
public:
    explicit FloatConst(std::string val) : Statement(STMT_FLOAT_CONST), val(std::move(val)) {}

private:
    std::string val;
};

}
}

#endif //ULTRA_RUBY_FLOATCONST_H
