#ifndef ULTRA_RUBY_AST_LOCAL_VARIABLE_H
#define ULTRA_RUBY_AST_LOCAL_VARIABLE_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class LocalVariable : public Statement {
public:
    LocalVariable(std::string name, int row, int col) :
            Statement(STMT_LOCAL_VARIABLE, row, col), name(std::move(name)) {}

    const std::string &getName() const {
        return name;
    }

private:
    std::string name;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_LOCAL_VARIABLE_H
