#ifndef ULTRA_RUBY_ASSIGNMENT_H
#define ULTRA_RUBY_ASSIGNMENT_H

#include <memory>
#include "Statement.h"
#include "Variable.h"

namespace UltraRuby {
namespace AST {

class Assignment : public Statement {
public:
    Assignment(std::unique_ptr<AST::Variable> dstPtr, std::unique_ptr<AST::Statement> src)
            : Statement(STMT_ASSIGNMENT), dstPtr(std::move(dstPtr)), src(std::move(src)) {}

private:
    std::unique_ptr<AST::Variable> dstPtr;
    std::unique_ptr<AST::Statement> src;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_ASSIGNMENT_H
