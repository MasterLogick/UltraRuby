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
    Call(std::string name, CallArgs *args, Statement *object)
            : Statement(STMT_CALL), name(std::move(name)), object(object), args(args) {}

    inline const std::string &getName() const {
        return name;
    }

    inline Statement *getObject() const {
        return object;
    }

    inline CallArgs *getArgs() const {
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
