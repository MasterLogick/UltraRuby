#ifndef ULTRA_RUBY_FUNCDEFARG_H
#define ULTRA_RUBY_FUNCDEFARG_H

#include <string>
#include <utility>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class FuncDefArg {
public:
    enum ArgType {
        AST_ARG_TYPE_NORMAL,
        AST_ARG_TYPE_VARIADIC,
        AST_ARG_TYPE_BLOCK,
        AST_ARG_TYPE_MAP,
    };

    FuncDefArg(std::string name, int pos, Statement *defaultValue, ArgType type)
            : name(std::move(name)), pos(pos), defaultValue(defaultValue), type(type) {}

    const std::string &getName() const {
        return name;
    }

    int getPos() const {
        return pos;
    }

    Statement *getDefaultValue() const {
        return defaultValue;
    }

    ArgType getType() const {
        return type;
    }

private:
    std::string name;
    int pos;
    Statement *defaultValue;
    ArgType type;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_FUNCDEFARG_H
