#ifndef ULTRA_RUBY_CALLARGS_H
#define ULTRA_RUBY_CALLARGS_H

#include <vector>
#include <memory>
#include "Statement.h"
#include "FunctionDef.h"

namespace UltraRuby {
namespace AST {

class CallArgs {
public:
    CallArgs(
            std::vector<std::unique_ptr<Statement>> args,
            std::unique_ptr<FunctionDef> block,
            bool parens,
            bool brackets)
            : args(std::move(args)), block(std::move(block)), parens(parens), brackets(brackets) {}

    inline const std::vector<std::unique_ptr<Statement>> &getArgs() const {
        return args;
    }

    inline const std::unique_ptr<FunctionDef> &getBlock() const {
        return block;
    }

    inline bool hasParens() const {
        return parens;
    }

    inline bool hasBrackets() const {
        return brackets;
    }

private:
    std::vector<std::unique_ptr<Statement>> args;
    std::unique_ptr<FunctionDef> block;
    bool parens;
    bool brackets;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_CALLARGS_H
