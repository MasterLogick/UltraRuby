#ifndef ULTRA_RUBY_AST_IF_H
#define ULTRA_RUBY_AST_IF_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class If : public Statement {
public:
    If(Statement *condition, Statement *trueBranch, Statement *falseBranch, int row, int col)
            : Statement(STMT_IF, row, col), condition(condition), trueBranch(trueBranch), falseBranch(falseBranch) {}

    Statement *getCondition() const {
        return condition;
    }

    Statement *getTrueBranch() const {
        return trueBranch;
    }

    Statement *getFalseBranch() const {
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
