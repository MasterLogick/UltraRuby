#ifndef ULTRA_RUBY_CODEGENERATOR_H
#define ULTRA_RUBY_CODEGENERATOR_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <map>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include "../ast/FloatConst.h"
#include "../ast/Variable.h"
#include "../ast/BinaryOperation.h"
#include "../ast/Call.h"
#include "../ast/Prototype.h"
#include "../ast/FunctionDef.h"

namespace UltraRuby {
namespace IR {

class CodeGenerator {
public:
    CodeGenerator();

    llvm::Value *codegenExpression(const std::unique_ptr<AST::Statement> &expression);

    llvm::Function *codegenFunctionDef(const std::unique_ptr<AST::FunctionDef> &functionDef);

private:

    void initPassManager();

    void logError(const char *error);

    llvm::Function *codegenPrototype(const std::unique_ptr<AST::Prototype> &prototype);

    llvm::Value *codegenNumber(const std::unique_ptr<AST::FloatConst> &number);

    llvm::Value *codegenVariable(const std::unique_ptr<AST::Variable> &variable);

    llvm::Value *codegenBinaryExpression(const std::unique_ptr<AST::BinaryOperation> &binExpr);

    llvm::Value *codegenCall(const std::unique_ptr<AST::Call> &call);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::Value *> scope;
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_CODEGENERATOR_H
