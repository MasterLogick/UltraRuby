#ifndef ULTRA_RUBY_AST_WHILE_H
#define ULTRA_RUBY_AST_WHILE_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class While : public Statement {
public:
    While(Statement *condition, Statement *body) : Statement(STMT_WHILE), condition(condition), body(body) {}

    Statement *getCondition() const {
        return condition;
    }

    Statement *getBody() const {
        return body;
    }

private:
    Statement *condition;
    Statement *body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_WHILE_H
