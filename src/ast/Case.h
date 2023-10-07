#ifndef ULTRA_RUBY_AST_CASE_H
#define ULTRA_RUBY_AST_CASE_H

#include <memory>
#include <vector>
#include "Statement.h"
#include "CaseWhereBlock.h"

namespace UltraRuby {
namespace AST {

class Case : public Statement {
public:
    Case(Statement *arg, std::vector<CaseWhereBlock *> cases)
            : Statement(STMT_CASE), arg(arg), cases(std::move(cases)) {}

    inline Statement *getArg() const {
        return arg;
    }

    inline const std::vector<CaseWhereBlock *> &getCases() const {
        return cases;
    }

private:
    Statement *arg;
    std::vector<CaseWhereBlock *> cases;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CASE_H
