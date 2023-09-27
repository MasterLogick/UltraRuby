#ifndef ULTRA_RUBY_CLASSINSTANCEDEF_H
#define ULTRA_RUBY_CLASSINSTANCEDEF_H

#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class ClassInstanceDef : public Statement {
public:
    ClassInstanceDef(std::unique_ptr<AST::Statement> instance, std::unique_ptr<AST::Statement> definition)
            : Statement(STMT_CLASS_INSTANCE_DEF), instance(std::move(instance)), definition(std::move(definition)) {}

private:
    std::unique_ptr<AST::Statement> instance;
    std::unique_ptr<AST::Statement> definition;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_CLASSINSTANCEDEF_H
