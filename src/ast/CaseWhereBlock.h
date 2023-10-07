#ifndef ULTRA_RUBY_CASEWHEREBLOCK_H
#define ULTRA_RUBY_CASEWHEREBLOCK_H

#include "Statement.h"
#include "Block.h"

namespace UltraRuby {
namespace AST {

class CaseWhereBlock {
public:
    CaseWhereBlock(Statement *cond, Block *block) : cond(cond), block(block) {}

    inline Statement *getCond() const {
        return cond;
    }

    inline Block *getBlock() const {
        return block;
    }

private:
    Statement *cond;
    Block *block;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_CASEWHEREBLOCK_H
