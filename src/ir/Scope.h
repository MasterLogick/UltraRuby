#ifndef ULTRA_RUBY_IR_SCOPE_H
#define ULTRA_RUBY_IR_SCOPE_H

#include <map>
#include <string>
#include <memory>
#include <llvm/IR/Value.h>
#include "../ast/FunctionDef.h"
#include "../ast/Variable.h"

namespace UltraRuby {
namespace IR {

class Scope {
public:
    Scope() : parent(nullptr) {}

    std::string deriveFunctionName(const std::unique_ptr<AST::FunctionDef> &functionDef);

    Scope *enterFunctionBody(const std::unique_ptr<AST::FunctionDef> &functionDef);

    llvm::Value *getVariable(const std::unique_ptr<AST::Variable> &var);

private:
    Scope *parent;
    std::map<std::string, int> vars;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_SCOPE_H
