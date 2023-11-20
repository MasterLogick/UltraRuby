#include "Scope.h"

namespace UltraRuby {
namespace IR {
void Scope::enterNewScope() {
    outer = new Scope(this);
}

void Scope::leaveScope() {
    vars = std::move(outer->vars);
    handlers = std::move(outer->handlers);
    outer = outer->outer;
}

llvm::Value *Scope::getLocalVariable(const std::string &name) {
    return vars[name];
}

void Scope::addLocalVariable(const std::string &name, llvm::Value *alloca) {
    vars[name] = alloca;
}

Scope::Scope(Scope *outer) {
    vars = std::move(outer->vars);
    handlers = std::move(outer->handlers);
    this->outer = outer->outer;
}

Scope::Scope() {
    outer = this;
}

void Scope::pushHandlerBlock(llvm::BasicBlock *pBlock) {
    handlers.push_back(pBlock);
}

void Scope::popHandlerBlock() {
    handlers.pop_back();
}

llvm::BasicBlock *Scope::getHandlerBlock() {
    if (handlers.empty())return nullptr;
    return handlers.back();
}
} // UltraRuby
} // IR