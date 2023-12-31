#ifndef ULTRA_RUBY_FUNCTIONGENERATOR_H
#define ULTRA_RUBY_FUNCTIONGENERATOR_H

#include <deque>
#include "CodeModule.h"
#include "../ast/FunctionDef.h"

namespace UltraRuby {
namespace IR {
class FunctionGenerator {
public:
    FunctionGenerator(CodeModule *module, AST::FunctionDef *functionDef);

    void emmitIR();

    void addVariable(const std::string &name, llvm::Value *ptr);

    llvm::Value *getVariable(const std::string &name);

    llvm::BasicBlock *getHandlerBlock();

    void pushHandlerBlock(llvm::BasicBlock *handlerBlock);

    void popHandlerBlock();

    CodeModule *getModule() const {
        return module;
    }

    llvm::Function *getFunc() const {
        return func;
    }

    AST::FunctionDef *getFunctionDef() const {
        return functionDef;
    }

    llvm::DISubprogram *getSubprogram() const {
        return subprogram;
    }

private:
    CodeModule *module;
    llvm::Function *func;
    AST::FunctionDef *functionDef;
    std::map<std::string, llvm::Value *> variables;
    std::deque<llvm::BasicBlock *> handlers;
    llvm::DISubprogram *subprogram;
};
} // UltraRuby
} // IR

#endif //ULTRA_RUBY_FUNCTIONGENERATOR_H
