#ifndef ULTRA_RUBY_BREAK_H
#define ULTRA_RUBY_BREAK_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Break : public Statement {
public:
    Break(std::unique_ptr<Statement> retVal) : Statement(STMT_BREAK), retVal(std::move(retVal)) {}

    inline const std::unique_ptr<Statement> &getRetVal() const {
        return retVal;
    }

private:
    std::unique_ptr<Statement> retVal;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_BREAK_H
