#ifndef ULTRA_RUBY_AST_RETURN_H
#define ULTRA_RUBY_AST_RETURN_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Return : public Statement {
public:
    Return(CallArgs *callArgs, int row, int col) : Statement(STMT_RETURN, row, col), callArgs(callArgs) {}

    CallArgs *getCallArgs() const {
        return callArgs;
    }

private:
    CallArgs *callArgs;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_RETURN_H
