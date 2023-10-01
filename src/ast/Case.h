#ifndef ULTRA_RUBY_AST_CASE_H
#define ULTRA_RUBY_AST_CASE_H

#include <memory>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Case : public Statement {
public:
    Case(
            std::unique_ptr<Statement> arg,
            std::vector<std::pair<std::unique_ptr<AST::Statement>, std::unique_ptr<AST::Statement>>> cases
    ) :
            Statement(STMT_CASE), arg(std::move(arg)), cases(std::move(cases)) {}

    inline const std::unique_ptr<Statement> &getArg() const {
        return arg;
    }

    inline const std::vector<std::pair<std::unique_ptr<AST::Statement>, std::unique_ptr<AST::Statement>>> &getCases() const {
        return cases;
    }

private:
    std::unique_ptr<Statement> arg;
    std::vector<std::pair<std::unique_ptr<AST::Statement>, std::unique_ptr<AST::Statement>>> cases;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CASE_H
