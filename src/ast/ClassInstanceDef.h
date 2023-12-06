#ifndef ULTRA_RUBY_AST_CLASSINSTANCEDEF_H
#define ULTRA_RUBY_AST_CLASSINSTANCEDEF_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class ClassInstanceDef : public Statement {
public:
    ClassInstanceDef(AST::Statement *instance, AST::Block *definition, int row, int col)
            : Statement(STMT_CLASS_INSTANCE_DEF, row, col), instance(instance), definition(definition) {}

    AST::Statement *getInstance() const {
        return instance;
    }

    AST::Block *getDefinition() const {
        return definition;
    }

private:
    AST::Statement *instance;
    AST::Block *definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CLASSINSTANCEDEF_H
