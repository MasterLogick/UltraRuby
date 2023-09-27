#ifndef ULTRA_RUBY_REDO_H
#define ULTRA_RUBY_REDO_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Redo : public Statement {
public:
    Redo() : Statement(STMT_REDO) {}
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_REDO_H
