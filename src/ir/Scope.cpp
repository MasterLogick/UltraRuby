#include "Scope.h"

namespace UltraRuby {
namespace IR {
std::string Scope::deriveFunctionName(AST::FunctionDef *functionDef) {
    return functionDef->getName();
}

void Scope::enterFunctionBody(AST::FunctionDef *functionDef) {
    outer = new Scope(this);
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

void Scope::addVariable(std::string name, llvm::Value *alloca) {
    vars[name] = alloca;
}

void Scope::markBlockAsTerminated() {

}

void Scope::leaveFunctionBody() {
    vars = std::move(outer->vars);
    outer = outer->outer;
}

Scope::Scope(Scope *outer) {
    vars = std::move(outer->vars);
    this->outer = outer;
}

Scope::Scope() {
    outer = this;
}

void Scope::pushHandlerBlock(llvm::BasicBlock *pBlock) {
    deque.push_back(pBlock);
}

void Scope::popHandlerBlock() {
    deque.pop_back();
}

llvm::BasicBlock *Scope::getRescueBlock() {
    if (deque.empty())return nullptr;
    return deque.back();
}
} // UltraRuby
} // IR