#ifndef ULTRA_RUBY_IR_SCOPE_H
#define ULTRA_RUBY_IR_SCOPE_H

#include <map>
#include <string>
#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <deque>
#include "../ast/FunctionDef.h"
#include "../ast/LocalVariable.h"
#include "../ast/ClassDef.h"
#include "../lang/Class.h"
#include "../ast/ModuleDef.h"
#include "../ast/ClassInstanceDef.h"

namespace UltraRuby {
namespace IR {

class Scope {
public:
    Scope();

    void enterNewScope();

    void leaveScope();

    void addLocalVariable(const std::string& name, llvm::Value *alloca);

    llvm::Value *getLocalVariable(const std::string &name);

    void pushHandlerBlock(llvm::BasicBlock *pBlock);

    void popHandlerBlock();

    llvm::BasicBlock *getHandlerBlock();

private:
    Scope(Scope *outer);

    Scope *outer;
    std::map<std::string, llvm::Value *> vars;
    std::deque<llvm::BasicBlock *> handlers;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_SCOPE_H
