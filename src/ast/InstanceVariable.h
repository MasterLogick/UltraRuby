#ifndef ULTRA_RUBY_INSTANCEVARIABLE_H
#define ULTRA_RUBY_INSTANCEVARIABLE_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class InstanceVariable : public Statement {
public:
    InstanceVariable(std::string name, int row, int col) :
            Statement(STMT_INSTANCE_VARIABLE, row, col), name(std::move(name)) {}

    const std::string &getName() const {
        return name;
    }

private:
    std::string name;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_INSTANCEVARIABLE_H
