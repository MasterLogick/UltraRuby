#ifndef ULTRA_RUBY_IR_CODEGENERATOR_H
#define ULTRA_RUBY_IR_CODEGENERATOR_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <map>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <set>
#include "Scope.h"
#include "../ast/ASTClassDecls.h"

namespace UltraRuby {
namespace Lang {
class Symbol;
}

namespace IR {

class CodeGenerator {
public:
    CodeGenerator();

    llvm::Function *codegenProgram(AST::FunctionDef *topLevelDecl);

    void debugPrintModuleIR();

    void setTarget(llvm::TargetMachine *machine, std::string triple);

    void runPass(llvm::legacy::PassManager &manager);

private:
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

    llvm::Function *codegenFunctionInternal(AST::FunctionDef *functionDef);

    llvm::Value *codegenLangCall(llvm::Function *langFunction, llvm::ArrayRef<llvm::Value *> args);

    llvm::Value *codegenCall(llvm::Value *caller,
                             llvm::Value *symVal,
                             int directArgsLen,
                             llvm::ArrayRef<llvm::Value *> args);

    bool codegenArgsProcessingPreamble(AST::FunctionDef *functionDef, llvm::Function *func);

    llvm::Value *codegenLangVariable(AST::LangVariable *variable);

    llvm::Value *codegenConstantRef(AST::ConstantRef *constantRef);

    llvm::Constant *codegenPointer(void *data);

    llvm::Constant *codegenConstPointer(const void *data);

    llvm::Value *codegenGetObjectClass(llvm::Value *object);

    llvm::Value *codegenSelf();

    llvm::Value *codegenCall(llvm::Value *caller, AST::Call *call);

    bool codegenInitializer();

    void declareExternLangFunctions();

    void logError(const char *error);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::GlobalVariable *> syms;
    std::map<std::string, llvm::GlobalVariable *> strs;
    Scope *scope;
    int suffix;

    llvm::PointerType *voidpTy;
    llvm::Type *int64Ty;
    llvm::Type *int8Ty;
    llvm::StructType *functionDefMetaType;
    llvm::StructType *exceptionHandlerType;

    llvm::Constant *nilConst;
    llvm::Constant *trueConst;
    llvm::Constant *falseConst;
    llvm::Constant *rootClass;

    llvm::Function *langArrayAlloc;
    llvm::Function *langHashAlloc;
    llvm::Function *langObjectCall[6];
    llvm::Function *langObjectCallB[6];
    llvm::Function *langObjectCallN[6];
    llvm::Function *langObjectCallNB[6];
    llvm::Function *langObjectCallV;
    llvm::Function *langObjectCallBV;
    llvm::Function *langObjectCallNV;
    llvm::Function *langObjectCallNBV;
    llvm::Function *langObjectDefineInstanceMethod;
    llvm::Function *langObjectDefineSingletonMethod;
    llvm::Function *langObjectDefineClassInstance;
    llvm::Function *langObjectDefineClass;
    llvm::Function *langObjectDefineModule;
    llvm::Function *langClassGetConst;
    llvm::Function *langClassSetConst;
    llvm::Function *langSymbolGet;

    llvm::Function *langStringGet;
    llvm::Function *cxaBeginCatch;
    llvm::Function *cxaEndCatch;
    llvm::Constant *gxxPersonalityV0;
    llvm::Function *cxaGetExceptionPtr;

    llvm::Constant *exceptionTypeInfo;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_CODEGENERATOR_H
