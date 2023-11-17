#ifndef ULTRA_RUBY_AST_CLASSDEF_H
#define ULTRA_RUBY_AST_CLASSDEF_H

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include "Statement.h"
#include "DefinitionName.h"

namespace UltraRuby {
namespace AST {

class ClassDef : public Statement {
public:
    ClassDef(Statement* classPos, Statement *superclass, AST::Block *definition)
            : Statement(STMT_CLASS_DEF), classPos(classPos), superclass(superclass), definition(definition) {}

    Statement *getClassPos() const {
        return classPos;
    }

    Statement *getSuperclass() const {
        return superclass;
    }

    AST::Block *getDefinition() const {
        return definition;
    }

private:
    Statement *classPos;
    Statement *superclass;
    Block *definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CLASSDEF_H
