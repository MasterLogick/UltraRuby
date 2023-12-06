#ifndef ULTRA_RUBY_AST_STATEMENT_H
#define ULTRA_RUBY_AST_STATEMENT_H

#include "StatementType.h"

namespace UltraRuby::AST {
class Statement {
public:
    Statement(StatementType type, int row, int col) : type(type), row(row), col(col) {}

    virtual ~Statement() = default;

    const StatementType type;
    int row;
    int col;
};

}


#endif //ULTRA_RUBY_AST_STATEMENT_H
