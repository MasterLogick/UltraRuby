#ifndef ULTRA_RUBY_UNARYOPERATION_H
#define ULTRA_RUBY_UNARYOPERATION_H

#include <memory>
#include "Statement.h"
#include "OperationType.h"

namespace UltraRuby {
namespace AST {

class UnaryOperation : public Statement {
public:
    explicit UnaryOperation(OperationType op, std::unique_ptr<AST::Statement> expr)
            : Statement(STMT_UN_OP), op(op), expr(std::move(expr)) {}

private:
    OperationType op;
    std::unique_ptr<AST::Statement> expr;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_UNARYOPERATION_H
