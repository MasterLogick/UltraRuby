#ifndef ULTRA_RUBY_AST_CLASSDEF_H
#define ULTRA_RUBY_AST_CLASSDEF_H

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class ClassDef : public Statement {
public:
    ClassDef(std::string className, std::string outerModule, std::string superclassIdentifier,
             AST::Block *definition) :
            Statement(STMT_CLASS_DEF),
            className(std::move(className)),
            outerModule(std::move(outerModule)),
            superclass(std::move(superclassIdentifier)),
            definition(definition) {}

    const std::string &getClassName() const {
        return className;
    }

    const std::string &getOuterModule() const {
        return outerModule;
    }

    const std::string &getSuperclass() const {
        return superclass;
    }

    AST::Block *getDefinition() const {
        return definition;
    }

private:
    std::string className;
    std::string outerModule;
    std::string superclass;
    AST::Block *definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CLASSDEF_H
