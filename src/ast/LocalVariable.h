#ifndef ULTRA_RUBY_AST_LOCAL_VARIABLE_H
#define ULTRA_RUBY_AST_LOCAL_VARIABLE_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class LocalVariable : public Statement {
public:
    explicit LocalVariable(std::string name) : name(std::move(name)), Statement(STMT_LOCAL_VARIABLE) {}

    inline const std::string &getName() const {
        return name;
    }

private:
    std::string name;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_LOCAL_VARIABLE_H
