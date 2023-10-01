#ifndef ULTRA_RUBY_AST_ASSIGNMENT_H
#define ULTRA_RUBY_AST_ASSIGNMENT_H

#include <memory>
#include "Statement.h"
#include "Variable.h"

namespace UltraRuby {
namespace AST {

class Assignment : public Statement {
public:
    Assignment(std::unique_ptr<AST::Variable> dst, std::unique_ptr<AST::Statement> src)
            : Statement(STMT_ASSIGNMENT), dst(std::move(dst)), src(std::move(src)) {}

    inline const std::unique_ptr<AST::Variable> &getDst() const {
        return dst;
    }

    inline const std::unique_ptr<AST::Statement> &getSrc() const {
        return src;
    }

private:
    std::unique_ptr<AST::Variable> dst;
    std::unique_ptr<AST::Statement> src;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_ASSIGNMENT_H
