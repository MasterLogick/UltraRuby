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
    ModuleDef(std::string moduleIdentifier, AST::Block *definition)
            : Statement(STMT_MODULE_DEF), moduleIdentifier(std::move(moduleIdentifier)), definition(definition) {}

    inline const std::string &getModuleIdentifier() const {
        return moduleIdentifier;
    }

    inline AST::Block *getDefinition() const {
        return definition;
    }

private:
    std::string moduleIdentifier;
    AST::Block *definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_MODULEDEF_H
