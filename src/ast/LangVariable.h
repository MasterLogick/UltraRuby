#ifndef ULTRA_RUBY_LANGVARIABLE_H
#define ULTRA_RUBY_LANGVARIABLE_H

#include "Statement.h"

namespace UltraRuby {
namespace AST {

class LangVariable : public Statement {
public:
    enum Type {
        LVT_SELF,
        LVT_NIL
    };

    LangVariable(Type type, int row, int col) : Statement(STMT_LANG_VARIABLE, row, col), type(type) {}

    Type getType() const {
        return type;
    }

private:

    Type type;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_LANGVARIABLE_H
