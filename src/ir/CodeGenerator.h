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

    void codegenProgram(const std::unique_ptr<AST::FunctionDef> &topLevelDecl);

    inline const std::unique_ptr<llvm::Module> &getModule() const {
        return module;
    }

    void debugPrintModuleIR();

    void setTarget(llvm::TargetMachine *machine, std::string triple);

    void runPass(llvm::legacy::PassManager &manager);

private:
    llvm::Value *codegenArray(const std::unique_ptr<AST::Array> &array);

    llvm::Value *codegenAssignment(const std::unique_ptr<AST::Assignment> &assignment);

    llvm::Value *codegenBinaryOperation(const std::unique_ptr<AST::BinaryOperation> &binOp);

    llvm::Value *codegenBlock(const std::unique_ptr<AST::Block> &block);

    llvm::Constant *codegenBoolConst(bool val);

    llvm::Value *codegenBreak();

    llvm::Value *codegenCall(const std::unique_ptr<AST::Call> &call);

    llvm::Value *codegenClassDef(const std::unique_ptr<AST::ClassDef> &classDef);

    llvm::Value *codegenClassInstanceDef(const std::unique_ptr<AST::ClassInstanceDef> &classInstanceDef);

    llvm::Value *exceptionalBlock(const std::unique_ptr<AST::ExceptionalBlock> &exceptionalBlock);

    llvm::Constant *codegenFloatConst(const std::unique_ptr<AST::FloatConst> &floatConst);

    llvm::Value *codegenFor(const std::unique_ptr<AST::For> &forAst);

    llvm::Value *codegenFunctionDef(const std::unique_ptr<AST::FunctionDef> &functionDef);

    llvm::Value *codegenHash(const std::unique_ptr<AST::Hash> &hash);

    llvm::Value *codegenIf(const std::unique_ptr<AST::If> &ifAst);

    llvm::Constant *codegenIntegerConst(const std::unique_ptr<AST::IntegerConst> &intConst);

    llvm::Value *codegenModuleDef(const std::unique_ptr<AST::ModuleDef> &moduleDef);

    llvm::Value *codegenNext();

    llvm::Value *codegenRedo();

    llvm::Value *codegenRetry();

    llvm::Value *codegenReturn(const std::unique_ptr<AST::Return> &returnAst);

    llvm::Value *codegenStatement(const std::unique_ptr<AST::Statement> &statement);

    llvm::Constant *codegenString(const std::unique_ptr<AST::String> &str);

    llvm::Constant *codegenSymbol(const std::unique_ptr<AST::Symbol> &symbol);

    llvm::Value *codegenUnaryOperation(const std::unique_ptr<AST::UnaryOperation> &unaryOperation);

    llvm::Value *codegenVariable(const std::unique_ptr<AST::Variable> &variable);

    llvm::Value *codegenWhile(const std::unique_ptr<AST::While> &whileAst);

    llvm::Value *codegenYield(const std::unique_ptr<AST::Yield> &yield);

    llvm::Value *codegenYieldSelf();


    void declareFunction(const std::unique_ptr<AST::FunctionDef> &functionDef);

    llvm::Function *codegenFunctionBody(const std::unique_ptr<AST::FunctionDef> &functionDef);

    bool codegenArgsProcessingPreamble(const std::unique_ptr<AST::FunctionDef> &functionDef);

    llvm::Value *codegenCall(llvm::Value *self, llvm::Constant *sym, std::vector<llvm::Value *> args);

    llvm::Constant *codegenPointer(void *data);

    void declareExternLangFunctions();

    void logError(const char *error);
//
//    llvm::Function *codegenPrototype(const std::unique_ptr<AST::Prototype> &prototype);
//
//    llvm::Value *codegenNumber(const std::unique_ptr<AST::FloatConst> &number);
//
//    llvm::Value *codegenVariable(const std::unique_ptr<AST::Variable> &variable);
//
//    llvm::Value *codegenBinaryExpression(const std::unique_ptr<AST::BinaryOperation> &binExpr);
//
//    llvm::Value *codegenCall(const std::unique_ptr<AST::Call> &call);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    Scope *scope;

    llvm::Function *langArrayAlloc;
    llvm::Function *langHashAlloc;
    llvm::Function *langObjectCall0;
    llvm::Function *langObjectCall1;
    llvm::Function *langObjectCall2;
    llvm::Function *langObjectCallV;
    llvm::Function *langObjectDefineInstanceMethod;
    llvm::Function *langObjectDefineSingletonMethod;
    llvm::Function *langClassDefineClass;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_IR_CODEGENERATOR_H
