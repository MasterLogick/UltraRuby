#ifndef ULTRA_RUBY_NEXT_H
#define ULTRA_RUBY_NEXT_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {
class Next : public Statement {
public:
    Next(std::unique_ptr<Statement> retVal) : Statement(STMT_NEXT), retVal(std::move(retVal)) {}

    inline const std::unique_ptr<Statement> &getRetVal() const {
        return retVal;
    }

private:
    std::unique_ptr<Statement> retVal;
};

} // UltraRuby
} // AST
#endif //ULTRA_RUBY_NEXT_H
