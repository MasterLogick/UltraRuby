#ifndef ULTRA_RUBY_AST_FUNCTIONDEF_H
#define ULTRA_RUBY_AST_FUNCTIONDEF_H

#include <memory>
#include <utility>
#include <cassert>
#include "Statement.h"
#include "Block.h"
#include "OptionalArg.h"

namespace UltraRuby {
namespace AST {

class FunctionDef : public Statement {
public:
    FunctionDef(std::string name,
                Block *body,
                std::vector<std::string> requiredArgsPrefix = {},
                std::vector<OptionalArg *> optionalArgs = {},
                std::string variadicArg = "",
                std::vector<std::string> requiredArgsSuffix = {},
                std::vector<OptionalArg *> namedArgs = {},
                std::string blockArg = "",
                Statement *singleton = nullptr) :
            Statement(STMT_FUNC_DEF),
            name(std::move(name)),
            requiredArgsPrefix(std::move(requiredArgsPrefix)),
            optionalArgs(std::move(optionalArgs)),
            variadicArg(std::move(variadicArg)),
            requiredArgsSuffix(std::move(requiredArgsSuffix)),
            namedArgs(std::move(namedArgs)),
            blockArg(std::move(blockArg)),
            singleton(singleton),
            body(body) {
        assert((this->optionalArgs.empty() &&
                this->variadicArg.empty() &&
                this->namedArgs.empty() &&
                this->blockArg.empty() &&
                this->requiredArgsSuffix.empty()) ||
               !this->requiredArgsSuffix.empty());
    }

    const std::string &getName() const {
        return name;
    }

    const std::vector<std::string> &getRequiredArgsPrefix() const {
        return requiredArgsPrefix;
    }

    const std::vector<OptionalArg *> &getOptionalArgs() const {
        return optionalArgs;
    }

    const std::string &getVariadicArg() const {
        return variadicArg;
    }

    const std::vector<std::string> &getRequiredArgsSuffix() const {
        return requiredArgsSuffix;
    }

    const std::vector<OptionalArg *> &getNamedArgs() const {
        return namedArgs;
    }

    const std::string &getBlockArg() const {
        return blockArg;
    }

    Statement *getSingleton() const {
        return singleton;
    }

    Block *getBody() const {
        return body;
    }

    int getArgc();

private:
    std::string name;
    std::vector<std::string> requiredArgsPrefix;
    std::vector<OptionalArg *> optionalArgs;
    std::string variadicArg;
    std::vector<std::string> requiredArgsSuffix;
    std::vector<OptionalArg *> namedArgs;
    std::string blockArg;
    Statement *singleton;
    Block *body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_FUNCTIONDEF_H
