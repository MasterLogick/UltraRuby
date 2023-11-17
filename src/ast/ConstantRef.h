#ifndef ULTRA_RUBY_CONSTANTREF_H
#define ULTRA_RUBY_CONSTANTREF_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class ConstantRef : public Statement {
public:
    ConstantRef(Statement *holder, std::string name) :
            Statement(STMT_CONSTANT_REF), holder(holder), name(std::move(name)) {}

    Statement *getHolder() const {
        return holder;
    }

    const std::string &getName() const {
        return name;
    }

private:
    Statement *holder;
    std::string name;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_CONSTANTREF_H
