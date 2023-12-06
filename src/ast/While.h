#ifndef ULTRA_RUBY_AST_WHILE_H
#define ULTRA_RUBY_AST_WHILE_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class While : public Statement {
public:
    While(Statement *condition, Statement *body, int row, int col) :
            Statement(STMT_WHILE, row, col), condition(condition), body(body) {}

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
