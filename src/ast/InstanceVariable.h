#ifndef ULTRA_RUBY_INSTANCEVARIABLE_H
#define ULTRA_RUBY_INSTANCEVARIABLE_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class InstanceVariable : public Statement {
public:
    explicit InstanceVariable(std::string name) : Statement(STMT_INSTANCE_VARIABLE), name(std::move(name)) {}

    const std::string &getName() const {
        return name;
    }

private:
    std::string name;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_INSTANCEVARIABLE_H
