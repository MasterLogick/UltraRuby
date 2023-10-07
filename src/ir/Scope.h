#ifndef ULTRA_RUBY_IR_SCOPE_H
#define ULTRA_RUBY_IR_SCOPE_H

#include <map>
#include <string>
#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
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
    Scope() : parent(nullptr) {}

    std::string deriveFunctionName(AST::FunctionDef *functionDef);

    Scope *enterFunctionBody(AST::FunctionDef *functionDef);

    llvm::Value *getVariable(const std::string &name);

    std::string deriveClassDeclFunction(UltraRuby::AST::ClassDef *pDef);

    Lang::Class *getSuperClass(AST::ClassDef *pDef);

    std::string deriveModuleDeclFunction(UltraRuby::AST::ModuleDef *module);

    std::string deriveClassInstanceMethod(UltraRuby::AST::ClassInstanceDef *classInstanceDef);

    Lang::Class **getOrAllocModule(AST::ModuleDef *moduleDef);

    Lang::Class **getOrAllocClass(AST::ClassDef *pDef);

    void addVariable(const std::string &name, llvm::Value *alloca);

    void markBlockAsTerminated();

private:
    Scope *parent;
    std::map<std::string, llvm::Value *> vars;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_SCOPE_H
