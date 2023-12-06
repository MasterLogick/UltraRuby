#ifndef ULTRA_RUBY_AST_BREAK_H
#define ULTRA_RUBY_AST_BREAK_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Break : public Statement {
public:
    Break(CallArgs *callArgs) : Statement(STMT_BREAK), callArgs(callArgs) {}

    CallArgs *getCallArgs() const {
        return callArgs;
    }

private:
    CallArgs *callArgs;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_BREAK_H
