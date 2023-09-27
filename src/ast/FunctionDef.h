#ifndef ULTRA_RUBY_FUNCTIONDEF_H
#define ULTRA_RUBY_FUNCTIONDEF_H

#include <memory>
#include "Prototype.h"
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

private:
    std::string name;
    std::unique_ptr<FuncDefArgs> args;
    std::unique_ptr<Statement> singleton;
    std::unique_ptr<Statement> body;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_FUNCTIONDEF_H
