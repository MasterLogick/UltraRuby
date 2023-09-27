#ifndef ULTRA_RUBY_BINARYOPERATION_H
#define ULTRA_RUBY_BINARYOPERATION_H

#include <memory>
#include "Statement.h"
#include "OperationType.h"

namespace UltraRuby {
namespace AST {

class BinaryOperation : public Statement {
public:
    BinaryOperation(OperationType op, std::unique_ptr<Statement> left, std::unique_ptr<Statement> right)
            : Statement(STMT_BIN_OP), op(op), left(std::move(left)), right(std::move(right)) {}

    inline char getOperator() const {
        return op;
    }

    inline const std::unique_ptr<Statement> &getLeft() const {
        return left;
    }

    inline const std::unique_ptr<Statement> &getRight() const {
        return right;
    }

private:
    OperationType op;
    std::unique_ptr<Statement> left;
    std::unique_ptr<Statement> right;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_BINARYOPERATION_H
