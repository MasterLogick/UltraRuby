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
    Case(Statement *arg, std::vector<CaseWhereBlock *> cases, int row, int col)
            : Statement(STMT_CASE, row, col), arg(arg), cases(std::move(cases)) {}

    Statement *getArg() const {
        return arg;
    }

    const std::vector<CaseWhereBlock *> &getCases() const {
        return cases;
    }

private:
    Statement *arg;
    std::vector<CaseWhereBlock *> cases;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CASE_H
