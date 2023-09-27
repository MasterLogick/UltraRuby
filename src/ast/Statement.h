#ifndef ULTRA_RUBY_STATEMENT_H
#define ULTRA_RUBY_STATEMENT_H

#include "StatementType.h"

namespace UltraRuby::AST {
class Statement {
public:
    explicit Statement(StatementType type) : type(type) {}

    virtual ~Statement() = default;

    StatementType type;
};

}


#endif //ULTRA_RUBY_STATEMENT_H
