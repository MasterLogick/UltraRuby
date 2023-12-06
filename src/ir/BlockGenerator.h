#ifndef ULTRA_RUBY_BLOCKGENERATOR_H
#define ULTRA_RUBY_BLOCKGENERATOR_H

#include "FunctionGenerator.h"
#include "../ast/ASTClassDecls.h"

namespace UltraRuby {
namespace IR {

class BlockGenerator {
public:
    explicit BlockGenerator(FunctionGenerator *functionGenerator);

    BlockGenerator(FunctionGenerator *functionGenerator, AST::Block *block);

    void emmitIR();

    llvm::Value *getReturnValue() const {
        return returnValue;
    }

    llvm::IRBuilder<> *getBuilder() {
        return builder;
    }

    llvm::BasicBlock *getEntryBlock() const {
        return entryBlock;
    }

protected:


    //================= Lang functions codegen start =================


    llvm::Value *codegenArray(AST::Array *array);

    llvm::Value *codegenBinaryOperation(AST::BinaryOperation *binOp);

    llvm::Value *codegenBlock(AST::Block *block);

    llvm::Value *codegenBoolConst(bool val);

    llvm::Value *codegenBreak(AST::Break *breakAst);

    llvm::Value *codegenCall(AST::Call *call);

    llvm::Value *codegenCase(AST::Case *caseAst);

    llvm::Value *codegenClassDef(AST::ClassDef *classDef);

    llvm::Value *codegenClassInstanceDef(AST::ClassInstanceDef *classInstanceDef);

    llvm::Value *codegenExceptionalBlock(AST::ExceptionalBlock *exceptionalBlock);

    llvm::Value *codegenFloatConst(AST::FloatConst *floatConst);

    llvm::Value *codegenFor(AST::For *forAst);

    llvm::Value *codegenFunctionDef(AST::FunctionDef *functionDef);

    llvm::Value *codegenHash(AST::Hash *hash);

    llvm::Value *codegenIf(AST::If *ifAst);

    llvm::Value *codegenIntegerConst(AST::IntegerConst *intConst);

    llvm::Value *codegenModuleDef(AST::ModuleDef *moduleDef);

    llvm::Value *codegenNext(AST::Next *next);

    llvm::Value *codegenRedo();

    llvm::Value *codegenRetry();

    llvm::Value *codegenReturn(AST::Return *returnAst);

    llvm::Value *codegenStatement(AST::Statement *statement);

    llvm::LoadInst *codegenString(AST::String *str);

    llvm::LoadInst *codegenSymbol(AST::Symbol *symbol);

    llvm::LoadInst *codegenSymbol(const std::string &symbol);

    llvm::Value *codegenUnaryOperation(AST::UnaryOperation *unaryOperation);

    llvm::Value *codegenLocalVariable(AST::LocalVariable *variable);

    llvm::Value *codegenWhile(AST::While *whileAst);

    llvm::Value *codegenYield(AST::Yield *yield);

    llvm::Value *codegenYieldSelf();

    llvm::Value *codegenInstanceVariable(AST::InstanceVariable *instanceVariable);


    //================= Lang functions codegen end =================


    llvm::Value *codegenCastToBoolInt1(llvm::Value *ptr);

    llvm::Value *codegenLangCall(llvm::Function *langFunction, llvm::ArrayRef<llvm::Value *> args);

    llvm::Value *codegenCall(llvm::Value *caller,
                             llvm::Value *symVal,
                             int directArgsLen,
                             llvm::ArrayRef<llvm::Value *> args);

    llvm::Value *codegenLangVariable(AST::LangVariable *variable);

    llvm::Value *codegenConstantRef(AST::ConstantRef *constantRef);

    llvm::Constant *codegenPointer(void *data);

    llvm::Value *codegenGetObjectClass(llvm::Value *object);

    llvm::Value *codegenSelf();

    llvm::Value *codegenCall(llvm::Value *caller, AST::Call *call);

    FunctionGenerator *functionGenerator;
    CodeModule *module;
    AST::Block *block;
    llvm::Value *returnValue;
    llvm::IRBuilder<> *builder;
    llvm::LLVMContext *context;
    llvm::PointerType *voidpTy;
    llvm::BasicBlock *entryBlock;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_BLOCKGENERATOR_H
