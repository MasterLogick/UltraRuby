#ifndef ULTRA_RUBY_AST_UNARYOPERATION_H
#define ULTRA_RUBY_AST_UNARYOPERATION_H

#include <memory>
#include "Statement.h"
#include "OperationType.h"

namespace UltraRuby {
namespace AST {

class UnaryOperation : public Statement {
public:
    UnaryOperation(OperationType op, AST::Statement *expr, int row, int col)
            : Statement(STMT_UN_OP, row, col), op(op), expr(expr) {}

    OperationType getOp() const {
        return op;
    }

    AST::Statement *getExpr() const {
        return expr;
    }

private:
    OperationType op;
    AST::Statement *expr;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_UNARYOPERATION_H
