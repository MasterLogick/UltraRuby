#ifndef ULTRA_RUBY_YIELD_H
#define ULTRA_RUBY_YIELD_H

#include <memory>
#include "Statement.h"
#include "CallArgs.h"

namespace UltraRuby {
namespace AST {

class Yield : public Statement {
public:
    Yield(std::unique_ptr<AST::CallArgs> args) : Statement(STMT_YIELD), args(std::move(args)) {}

private:
    std::unique_ptr<AST::CallArgs> args;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_YIELD_H