#ifndef ULTRA_RUBY_IR_SCOPE_H
#define ULTRA_RUBY_IR_SCOPE_H

#include <map>
#include <string>
#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <deque>
#include "../ast/FunctionDef.h"
#include "../ast/Variable.h"
#include "../ast/ClassDef.h"
#include "../lang/Class.h"
#include "../ast/ModuleDef.h"
#include "../ast/ClassInstanceDef.h"

namespace UltraRuby {
namespace IR {

class Scope {
public:
    Scope();

    std::string deriveFunctionName(AST::FunctionDef *functionDef);

    void enterFunctionBody(AST::FunctionDef *functionDef);

    void leaveFunctionBody();

    llvm::Value *getVariable(const std::string &name);

    std::string deriveClassDeclFunction(UltraRuby::AST::ClassDef *pDef);

    Lang::Class *getSuperClass(AST::ClassDef *pDef);

    std::string deriveModuleDeclFunction(UltraRuby::AST::ModuleDef *module);

    std::string deriveClassInstanceMethod(UltraRuby::AST::ClassInstanceDef *classInstanceDef);

    Lang::Class **getOrAllocModule(AST::ModuleDef *moduleDef);

    Lang::Class **getOrAllocClass(AST::ClassDef *pDef);

    void addVariable(std::string name, llvm::Value *alloca);

    void markBlockAsTerminated();

    void pushHandlerBlock(llvm::BasicBlock *pBlock);

    void popHandlerBlock();

    llvm::BasicBlock *getRescueBlock();

private:
    Scope(Scope *outer);

    Scope *outer;
    std::map<std::string, llvm::Value *> vars;
    std::deque<llvm::BasicBlock *> deque;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_SCOPE_H
