#ifndef ULTRA_RUBY_AST_YIELD_H
#define ULTRA_RUBY_AST_YIELD_H

#include <memory>
#include "Statement.h"
#include "CallArgs.h"

namespace UltraRuby {
namespace AST {

class Yield : public Statement {
public:
    Yield(AST::CallArgs *args, int row, int col) : Statement(STMT_YIELD, row, col), args(args) {}

    AST::CallArgs *getArgs() const {
        return args;
    }

private:
    AST::CallArgs *args;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_YIELD_H
