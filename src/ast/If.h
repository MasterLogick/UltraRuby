#ifndef ULTRA_RUBY_IF_H
#define ULTRA_RUBY_IF_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class If : public Statement {
public:
    If(std::unique_ptr<Statement> condition, std::unique_ptr<Statement> trueBranch,
       std::unique_ptr<Statement> falseBranch)
            : Statement(STMT_IF), condition(std::move(condition)), trueBranch(std::move(trueBranch)),
              falseBranch(std::move(falseBranch)) {}

    inline const std::unique_ptr<Statement> &getCondition() const {
        return condition;
    }

    inline const std::unique_ptr<Statement> &getTrueBranch() const {
        return trueBranch;
    }

    inline const std::unique_ptr<Statement> &getFalseBranch() const {
        return falseBranch;
    }

private:
    std::unique_ptr<Statement> condition;
    std::unique_ptr<Statement> trueBranch;
    std::unique_ptr<Statement> falseBranch;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_IF_H
