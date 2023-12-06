#ifndef ULTRA_RUBY_AST_CALLARGS_H
#define ULTRA_RUBY_AST_CALLARGS_H

#include <utility>
#include <vector>
#include <memory>
#include "Statement.h"
#include "FunctionDef.h"

namespace UltraRuby {
namespace AST {

class CallArgs {
public:
    CallArgs(std::vector<Statement *> args,
             std::map<std::string, Statement *> namedArgs,
             FunctionDef *block,
             bool brackets,
             bool parenthesis)
            : args(std::move(args)),
              namedArgs(std::move(namedArgs)),
              block(block),
              brackets(brackets),
              parenthesis(parenthesis) {}

    const std::vector<Statement *> &getArgs() const {
        return args;
    }

    const std::map<std::string, Statement *> &getNamedArgs() const {
        return namedArgs;
    }

    FunctionDef *getBlock() const {
        return block;
    }

    bool hasBrackets() const {
        return brackets;
    }

    bool hasParenthesis() const {
        return parenthesis;
    }

private:
    std::vector<Statement *> args;
    std::map<std::string, Statement *> namedArgs;
    FunctionDef *block;
    bool brackets;
    bool parenthesis;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CALLARGS_H
