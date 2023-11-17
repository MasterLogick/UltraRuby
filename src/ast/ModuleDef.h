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
    ModuleDef(std::string moduleName, std::string outerModule, AST::Block *definition)
            : Statement(STMT_MODULE_DEF), moduleName(std::move(moduleName)),
              outerModule(std::move(outerModule)), definition(definition) {}

    const std::string &getModuleName() const {
        return moduleName;
    }

    const std::string &getOuterModule() const {
        return outerModule;
    }

    AST::Block *getDefinition() const {
        return definition;
    }

private:
    std::string moduleName;
    std::string outerModule;
    AST::Block *definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_MODULEDEF_H
