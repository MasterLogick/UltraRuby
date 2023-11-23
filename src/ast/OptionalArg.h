#ifndef ULTRA_RUBY_OPTIONALARG_H
#define ULTRA_RUBY_OPTIONALARG_H

#include <string>
#include <utility>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class OptionalArg {
public:
    OptionalArg(std::string name, Statement *defaultValue) : name(std::move(name)), defaultValue(defaultValue) {}

    const std::string &getName() const {
        return name;
    }

    Statement *getDefaultValue() const {
        return defaultValue;
    }

private:
    std::string name;
    Statement *defaultValue;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_OPTIONALARG_H
