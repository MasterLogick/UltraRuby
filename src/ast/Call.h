#ifndef ULTRA_RUBY_CALL_H
#define ULTRA_RUBY_CALL_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "Statement.h"
#include "CallArgs.h"

namespace UltraRuby {
namespace AST {

class Call : public Statement {
public:
    Call(std::string name, std::unique_ptr<CallArgs> args, std::unique_ptr<Statement> callee)
            : Statement(STMT_CALL), name(std::move(name)), callee(std::move(callee)), args(std::move(args)) {}

private:
    std::string name;
    std::unique_ptr<Statement> callee;
    std::unique_ptr<CallArgs> args;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_CALL_H
