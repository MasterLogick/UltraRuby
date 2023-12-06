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
        throw CodegenException("function redeclaration", functionDef->row, functionDef->col);
    }
    auto *voidpTy = module->getVoidpTy();

    llvm::FunctionType *ft;
    int argc = functionDef->getArgc();
    llvm::DISubroutineType *diST;
    if (argc == -1) {
        ft = llvm::FunctionType::get(voidpTy, {voidpTy, llvm::IntegerType::getInt32Ty(*module->getContext()), voidpTy},
                                     false);
        diST = module->getVariadicSubroutineDTy();
    } else {
        ft = llvm::FunctionType::get(voidpTy, std::vector<llvm::Type *>(argc + 1, voidpTy), false);
        diST = module->getDiBuilder()->createSubroutineType(module->getDiBuilder()->getOrCreateTypeArray(
                std::vector<llvm::Metadata *>(argc + 1, module->getObjectPtrDTy())));
    }
    func = llvm::Function::Create(ft, llvm::Function::PrivateLinkage, functionDef->getName(), module->getLLVMModule());

    subprogram = module->getDiBuilder()->createFunction(
            module->getDiCurrentFile(), functionDef->getName(), functionDef->getName(), module->getDiCurrentFile(),
            functionDef->row, diST, functionDef->getBody()->row, llvm::DINode::FlagPrototyped,
            llvm::DISubprogram::SPFlagDefinition);
    func->setSubprogram(subprogram);
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