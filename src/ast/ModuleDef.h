#ifndef ULTRA_RUBY_AST_MODULEDEF_H
#define ULTRA_RUBY_AST_MODULEDEF_H

#include <string>
#include <memory>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class ModuleDef : public Statement {
public:
    ModuleDef(Statement *modulePos, AST::Block *definition, int row, int col)
            : Statement(STMT_MODULE_DEF, row, col), modulePos(modulePos), definition(definition) {}

    Statement *getModulePos() const {
        return modulePos;
    }

    AST::Block *getDefinition() const {
        return definition;
    }

private:
    Statement *modulePos;
    AST::Block *definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_MODULEDEF_H
