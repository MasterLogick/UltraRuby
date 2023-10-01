#ifndef ULTRA_RUBY_AST_FUNCTIONDEF_H
#define ULTRA_RUBY_AST_FUNCTIONDEF_H

#include <memory>
#include "Statement.h"
#include "FuncDefArgs.h"

namespace UltraRuby {
namespace AST {

class FunctionDef : public Statement {
public:
    FunctionDef(
            std::string name,
            std::unique_ptr<FuncDefArgs> args,
            std::unique_ptr<Statement> singleton,
            std::unique_ptr<Statement> body
    ) : Statement(STMT_FUNC_DEF),
        name(std::move(name)),
        args(std::move(args)),
        singleton(std::move(singleton)),
        body(std::move(body)) {}

    inline const std::string &getName() const {
        return name;
    }

    inline const std::unique_ptr<FuncDefArgs> &getArgs() const {
        return args;
    }

    inline const std::unique_ptr<Statement> &getSingleton() const {
        return singleton;
    }

    inline const std::unique_ptr<Statement> &getBody() const {
        return body;
    }

private:
    std::string name;
    std::unique_ptr<FuncDefArgs> args;
    std::unique_ptr<Statement> singleton;
    std::unique_ptr<Statement> body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_FUNCTIONDEF_H
