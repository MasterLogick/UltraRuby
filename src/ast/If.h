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

private:
    std::unique_ptr<Statement> condition;
    std::unique_ptr<Statement> trueBranch;
    std::unique_ptr<Statement> falseBranch;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_IF_H
