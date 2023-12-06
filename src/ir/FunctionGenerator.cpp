#include "FunctionGenerator.h"
#include "BlockGenerator.h"
#include "CodegenException.h"
#include "PreambleGenerator.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>

namespace UltraRuby {
namespace IR {
FunctionGenerator::FunctionGenerator(CodeModule *module, AST::FunctionDef *functionDef) :
        module(module), functionDef(functionDef), func(nullptr) {}

void FunctionGenerator::emmitIR() {
    func = module->getLLVMModule()->getFunction(functionDef->getName());
    if (func != nullptr) {
        throw CodegenException("function redeclaration");
    }
    auto *voidpTy = module->getVoidpTy();

    llvm::FunctionType *ft;
    int argc = functionDef->getArgc();
    if (argc == -1) {
        ft = llvm::FunctionType::get(voidpTy, {voidpTy, llvm::IntegerType::getInt32Ty(*module->getContext()), voidpTy},
                                     false);
    } else {
        ft = llvm::FunctionType::get(voidpTy, std::vector<llvm::Type *>(argc + 1, voidpTy), false);
    }
    func = llvm::Function::Create(ft, llvm::Function::PrivateLinkage, functionDef->getName(), module->getLLVMModule());

    try {
        PreambleGenerator pg(this);
        pg.emmitIR();
        BlockGenerator bg(this, functionDef->getBody());
        bg.emmitIR();
        pg.getBuilder()->CreateBr(bg.getEntryBlock());
        auto retVal = bg.getReturnValue();
        bg.getBuilder()->CreateRet(retVal);
    } catch (...) {
        func->eraseFromParent();
        throw;
    }
    llvm::verifyFunction(*func);
}

void FunctionGenerator::addVariable(const std::string &name, llvm::Value *ptr) {
    variables[name] = ptr;
}

llvm::Value *FunctionGenerator::getVariable(const std::string &name) {
    return variables[name];
}

llvm::BasicBlock *FunctionGenerator::getHandlerBlock() {
    if (handlers.empty()) return nullptr;
    return handlers.back();
}

void FunctionGenerator::pushHandlerBlock(llvm::BasicBlock *handlerBlock) {
    handlers.push_back(handlerBlock);
}

void FunctionGenerator::popHandlerBlock() {
    handlers.pop_back();
}
} // UltraRuby
} // IR