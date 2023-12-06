#ifndef ULTRA_RUBY_AST_RETRY_H
#define ULTRA_RUBY_AST_RETRY_H

#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Retry : public Statement {
public:
    Retry(int row, int col) : Statement(STMT_RETRY, row, col) {}
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_RETRY_H
