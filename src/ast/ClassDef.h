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
    ClassDef(
            std::string classIdentifier,
            std::string superclassIdentifier,
            std::unique_ptr<AST::Statement> definition
    ) :
            Statement(STMT_CLASS_DEF),
            classIdentifier(std::move(classIdentifier)),
            superclassIdentifier(std::move(superclassIdentifier)),
            definition(std::move(definition)) {}

    inline const std::string &getClassIdentifier() const {
        return classIdentifier;
    }

    inline const std::string &getSuperclassIdentifier() const {
        return superclassIdentifier;
    }

    inline const std::unique_ptr<AST::Statement> &getDefinition() const {
        return definition;
    }

private:
    std::string classIdentifier;
    std::string superclassIdentifier;
    std::unique_ptr<AST::Statement> definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CLASSDEF_H
