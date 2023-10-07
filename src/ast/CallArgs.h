#ifndef ULTRA_RUBY_AST_CALLARGS_H
#define ULTRA_RUBY_AST_CALLARGS_H

#include <vector>
#include <memory>
#include "Statement.h"
#include "FunctionDef.h"

namespace UltraRuby {
namespace AST {

class CallArgs {
public:
    CallArgs(std::vector<Statement *> args, FunctionDef *block, bool parens, bool brackets)
            : args(std::move(args)), block(block), parens(parens), brackets(brackets) {}

    inline const std::vector<Statement *> &getArgs() const {
        return args;
    }

    inline FunctionDef *getBlock() const {
        return block;
    }

    inline bool hasParens() const {
        return parens;
    }

    inline bool hasBrackets() const {
        return brackets;
    }

private:
    std::vector<Statement *> args;
    FunctionDef *block;
    bool parens;
    bool brackets;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CALLARGS_H
