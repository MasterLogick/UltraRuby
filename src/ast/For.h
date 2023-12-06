#ifndef ULTRA_RUBY_AST_FOR_H
#define ULTRA_RUBY_AST_FOR_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class For : public Statement {
public:
    For(Statement *var, Statement *expr, Statement *body) : Statement(STMT_FOR), var(var), expr(expr), body(body) {}

    Statement *getVar() const {
        return var;
    }

    Statement *getExpr() const {
        return expr;
    }

    Statement *getBody() const {
        return body;
    }

private:
    Statement *var;
    Statement *expr;
    Statement *body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_FOR_H
