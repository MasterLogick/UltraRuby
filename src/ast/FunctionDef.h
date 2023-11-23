#ifndef ULTRA_RUBY_AST_FUNCTIONDEF_H
#define ULTRA_RUBY_AST_FUNCTIONDEF_H

#include <memory>
#include "Statement.h"
#include "Block.h"
#include "FuncDefArg.h"

namespace UltraRuby {
namespace AST {

class FunctionDef : public Statement {
public:
    FunctionDef(std::string name, std::vector<FuncDefArg *> args, Statement *singleton, Block *body);

    inline const std::string &getName() const {
        return name;
    }

    const std::vector<FuncDefArg *> &getArgs() const {
        return args;
    }

    inline Statement *getSingleton() const {
        return singleton;
    }

    inline Block *getBody() const {
        return body;
    }

    int getArgc();

    bool hasNamedArgs();

    bool hasBlockArg();

    FuncDefArg *getBlockArg();

private:
    std::string name;
    std::vector<FuncDefArg *> args;
    Statement *singleton;
    Block *body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_FUNCTIONDEF_H
