#ifndef ULTRA_RUBY_IR_CODEGENERATOR_H
#define ULTRA_RUBY_IR_CODEGENERATOR_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <map>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Target/TargetMachine.h>
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
    llvm::Value *codegenArray(AST::Array *array);

    llvm::Value *codegenBinaryOperation(AST::BinaryOperation *binOp);

    llvm::Value *codegenBlock(AST::Block *block);

    llvm::Constant *codegenBoolConst(bool val);

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

    llvm::Constant *codegenString(AST::String *str);

    llvm::Constant *codegenSymbol(AST::Symbol *symbol);

    llvm::Value *codegenUnaryOperation(AST::UnaryOperation *unaryOperation);

    llvm::Value *codegenVariable(AST::Variable *variable);

    llvm::Value *codegenWhile(AST::While *whileAst);

    llvm::Value *codegenYield(AST::Yield *yield);

    llvm::Value *codegenYieldSelf();


    llvm::Value *codegenCastToBoolInt1(llvm::Value *ptr);

    llvm::Function *codegenFunctionBody(AST::FunctionDef *functionDef);

    llvm::Function *codegenFunction(AST::FunctionDef *functionDef);

    llvm::Value *codegenLangCall(llvm::Function *langFunction, std::vector<llvm::Value *> args);

    bool codegenArgsProcessingPreamble(AST::FunctionDef *functionDef, llvm::Function *func);

    llvm::Constant *codegenPointer(void *data);

    void declareExternLangFunctions();

    void logError(const char *error);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    Scope *scope;

    llvm::PointerType *voidpTy;
    llvm::Type *int64Ty;

    llvm::Constant *nilConst;
    llvm::Constant *trueConst;
    llvm::Constant *falseConst;

    llvm::Function *langArrayAlloc;
    llvm::Function *langHashAlloc;
    llvm::Function *langObjectCall0;
    llvm::Function *langObjectCall1;
    llvm::Function *langObjectCall2;
    llvm::Function *langObjectCallV;
    llvm::Function *langObjectDefineInstanceMethod;
    llvm::Function *langObjectDefineSingletonMethod;
    llvm::Function *langClassDefineClass;
    llvm::Function *langModuleDefineModule;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_CODEGENERATOR_H
