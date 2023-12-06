#ifndef ULTRA_RUBY_AST_BINARYOPERATION_H
#define ULTRA_RUBY_AST_BINARYOPERATION_H

#include <memory>
#include "Statement.h"
#include "OperationType.h"

namespace UltraRuby {
namespace AST {

class BinaryOperation : public Statement {
public:
    BinaryOperation(OperationType op, Statement *left, Statement *right)
            : Statement(STMT_BIN_OP), op(op), left(left), right(right) {}

    OperationType getOperator() const {
        return op;
    }

    Statement *getLeft() const {
        return left;
    }

    Statement *getRight() const {
        return right;
    }

private:
    OperationType op;
    Statement *left;
    Statement *right;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_BINARYOPERATION_H
