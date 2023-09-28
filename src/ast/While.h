#ifndef ULTRA_RUBY_WHILE_H
#define ULTRA_RUBY_WHILE_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class While : public Statement {
public:
    While(std::unique_ptr<Statement> condition, std::unique_ptr<Statement> body)
            : Statement(STMT_WHILE), condition(std::move(condition)), body(std::move(body)) {}

    inline const std::unique_ptr<Statement> &getCondition() const {
        return condition;
    }

    inline const std::unique_ptr<Statement> &getBody() const {
        return body;
    }

private:
    std::unique_ptr<Statement> condition;
    std::unique_ptr<Statement> body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_WHILE_H
