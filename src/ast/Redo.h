#ifndef ULTRA_RUBY_AST_REDO_H
#define ULTRA_RUBY_AST_REDO_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Redo : public Statement {
public:
    Redo(int row, int col) : Statement(STMT_REDO, row, col) {}
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_REDO_H
