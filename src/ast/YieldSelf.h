#ifndef ULTRA_RUBY_AST_YIELDSELF_H
#define ULTRA_RUBY_AST_YIELDSELF_H

#include "Statement.h"

namespace UltraRuby {
namespace AST {

class YieldSelf : public Statement {
public:
    YieldSelf(int row, int col) : Statement(STMT_YIELD_SELF, row, col) {}
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_YIELDSELF_H
