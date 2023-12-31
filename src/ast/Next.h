#ifndef ULTRA_RUBY_AST_NEXT_H
#define ULTRA_RUBY_AST_NEXT_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {
class Next : public Statement {
public:
    Next(CallArgs *callArgs, int row, int col) : Statement(STMT_NEXT, row, col), callArgs(callArgs) {}

    CallArgs *getCallArgs() const {
        return callArgs;
    }

private:
    CallArgs *callArgs;
};

} // UltraRuby
} // AST
#endif //ULTRA_RUBY_AST_NEXT_H
