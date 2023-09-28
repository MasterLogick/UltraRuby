#ifndef ULTRA_RUBY_RETURN_H
#define ULTRA_RUBY_RETURN_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Return : public Statement {
public:
    Return(std::unique_ptr<Statement> retVal) : Statement(STMT_RETURN), retVal(std::move(retVal)) {}

    inline const std::unique_ptr<Statement> &getRetVal() const {
        return retVal;
    }

private:
    std::unique_ptr<Statement> retVal;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_RETURN_H
