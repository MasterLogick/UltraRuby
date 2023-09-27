#ifndef ULTRA_RUBY_VARIABLE_H
#define ULTRA_RUBY_VARIABLE_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Variable : public Statement {
public:
    explicit Variable(std::string name) : name(std::move(name)), Statement(STMT_VARIABLE) {}

    inline const std::string &getName() const {
        return name;
    }

private:
    std::string name;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_VARIABLE_H
