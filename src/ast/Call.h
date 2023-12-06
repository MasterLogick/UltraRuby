#ifndef ULTRA_RUBY_AST_CALL_H
#define ULTRA_RUBY_AST_CALL_H

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
    Call(std::string name, CallArgs *args, Statement *object, int row, int col)
            : Statement(STMT_CALL, row, col), name(std::move(name)), object(object), args(args) {
        assert((args->hasBrackets() && name.empty() || !args->hasBrackets()) && "bracket call must have empty name");
    }

    const std::string &getName() const {
        return name;
    }

    Statement *getObject() const {
        return object;
    }

    CallArgs *getArgs() const {
        return args;
    }

private:
    std::string name;
    Statement *object;
    CallArgs *args;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CALL_H
