#ifndef ULTRA_RUBY_AST_IF_H
#define ULTRA_RUBY_AST_IF_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class If : public Statement {
public:
    If(Statement *condition, Statement *trueBranch, Statement *falseBranch)
            : Statement(STMT_IF), condition(condition), trueBranch(trueBranch), falseBranch(falseBranch) {}

    inline Statement *getCondition() const {
        return condition;
    }

    inline Statement *getTrueBranch() const {
        return trueBranch;
    }

    inline Statement *getFalseBranch() const {
        return falseBranch;
    }

private:
    Statement *condition;
    Statement *trueBranch;
    Statement *falseBranch;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_IF_H
