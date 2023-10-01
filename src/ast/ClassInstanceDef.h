#ifndef ULTRA_RUBY_AST_CLASSINSTANCEDEF_H
#define ULTRA_RUBY_AST_CLASSINSTANCEDEF_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class ClassInstanceDef : public Statement {
public:
    ClassInstanceDef(std::unique_ptr<AST::Statement> instance, std::unique_ptr<AST::Statement> definition)
            : Statement(STMT_CLASS_INSTANCE_DEF), instance(std::move(instance)), definition(std::move(definition)) {}

    inline const std::unique_ptr<AST::Statement> &getInstance() const {
        return instance;
    }

    inline const std::unique_ptr<AST::Statement> &getDefinition() const {
        return definition;
    }

private:
    std::unique_ptr<AST::Statement> instance;
    std::unique_ptr<AST::Statement> definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_CLASSINSTANCEDEF_H
