#ifndef ULTRA_RUBY_FOR_H
#define ULTRA_RUBY_FOR_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class For : public Statement {
public:
    For(std::unique_ptr<Statement> var, std::unique_ptr<Statement> expr, std::unique_ptr<Statement> body)
            : Statement(STMT_FOR), var(std::move(var)), expr(std::move(expr)), body(std::move(body)) {}

    inline const std::unique_ptr<Statement> &getVar() const {
        return var;
    }

    inline const std::unique_ptr<Statement> &getExpr() const {
        return expr;
    }

    inline const std::unique_ptr<Statement> &getBody() const {
        return body;
    }

private:
    std::unique_ptr<Statement> var;
    std::unique_ptr<Statement> expr;
    std::unique_ptr<Statement> body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_FOR_H
