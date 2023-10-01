#include "Scope.h"

namespace UltraRuby {
namespace IR {
std::string Scope::deriveFunctionName(const std::unique_ptr<AST::FunctionDef> &functionDef) {
    return functionDef->getName();
}

Scope *Scope::enterFunctionBody(const std::unique_ptr<AST::FunctionDef> &functionDef) {
    return this;
}

llvm::Value *Scope::getVariable(const std::unique_ptr<AST::Variable> &var) {
    return nullptr;
}
} // UltraRuby
} // IR