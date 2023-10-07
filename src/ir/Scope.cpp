#include "Scope.h"

namespace UltraRuby {
namespace IR {
std::string Scope::deriveFunctionName(AST::FunctionDef *functionDef) {
    return functionDef->getName();
}

Scope *Scope::enterFunctionBody(AST::FunctionDef *functionDef) {
    return this;
}

std::string Scope::deriveClassDeclFunction(UltraRuby::AST::ClassDef *pDef) {
    return std::string();
}

Lang::Class *Scope::getSuperClass(AST::ClassDef *pDef) {
    return nullptr;
}

std::string Scope::deriveModuleDeclFunction(UltraRuby::AST::ModuleDef *module) {
    return std::string();
}

std::string Scope::deriveClassInstanceMethod(UltraRuby::AST::ClassInstanceDef *classInstanceDef) {
    return std::string();
}

Lang::Class **Scope::getOrAllocModule(AST::ModuleDef *moduleDef) {
    return nullptr;
}

Lang::Class **Scope::getOrAllocClass(AST::ClassDef *pDef) {
    return nullptr;
}

llvm::Value *Scope::getVariable(const std::string &name) {
    return vars[name];
}

void Scope::addVariable(const std::string &name, llvm::Value *alloca) {
    vars[name] = alloca;
}

void Scope::markBlockAsTerminated() {

}
} // UltraRuby
} // IR