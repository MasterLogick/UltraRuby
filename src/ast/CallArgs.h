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
    CallArgs(std::vector<Statement *> args, std::map<std::string, Statement *> namedArgs, FunctionDef *block,
             bool brackets)
            : args(std::move(args)), namedArgs(std::move(namedArgs)), block(block), brackets(brackets) {}

    inline const std::vector<Statement *> &getArgs() const {
        return args;
    }

    const std::map<std::string, Statement *> &getNamedArgs() const {
        return namedArgs;
    }

    inline FunctionDef *getBlock() const {
        return block;
    }

    inline bool hasBrackets() const {
        return brackets;
    }

private:
    std::vector<Statement *> args;
    std::map<std::string, Statement *> namedArgs;
    FunctionDef *block;
    bool brackets;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CALLARGS_H
