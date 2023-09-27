/*
#include <iostream>
#include "CodeGenerator.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/InstSimplifyPass.h>
#include <llvm/Transforms/Scalar/GVN.h>


namespace UltraRuby {
namespace IR {
void CodeGenerator::logError(const char *error) {
    std::cerr << "Code generator error: " << error << std::endl;
}

llvm::Value *CodeGenerator::codegenExpression(const std::unique_ptr<AST::Statement> &expression) {
    switch (expression->type) {
        case AST::STMT_CALL:
            return codegenCall((const std::unique_ptr<AST::Call> &) expression);
        case AST::STMT_VARIABLE:
            return codegenVariable((const std::unique_ptr<AST::Variable> &) expression);
        case AST::STMT_NUMBER:
            return codegenNumber((const std::unique_ptr<AST::FloatConst> &) expression);
        case AST::STMT_BIN_OP:
            return codegenBinaryExpression((const std::unique_ptr<AST::BinaryOperation> &) expression);
        case AST::STMT_UNKNOWN:
        default:
            logError("unknown expression");
            return nullptr;
    }
}

llvm::Value *CodeGenerator::codegenNumber(const std::unique_ptr<AST::FloatConst> &number) {
    return llvm::ConstantFP::get(*context, llvm::APFloat(number->getVal()));
}

llvm::Value *CodeGenerator::codegenVariable(const std::unique_ptr<AST::Variable> &variable) {
    auto *v = scope[variable.];
    if (v == nullptr) {
        logError("unknown variable");
        return nullptr;
    }
    return v;
}

llvm::Value *CodeGenerator::codegenBinaryExpression(const std::unique_ptr<AST::BinaryOperation> &binExpr) {
    auto *l = codegenExpression(binExpr->getLeft());
    if (l == nullptr) {
        return nullptr;
    }
    auto *r = codegenExpression(binExpr->getRight());
    if (r == nullptr) {
        return nullptr;
    }
    switch (binExpr->getOperator()) {
        case AST::OperationType::BINOP_LESS: {
            auto *val = builder->CreateFCmpULT(l, r);
            return builder->CreateUIToFP(val, llvm::Type::getDoubleTy(*context));
        }
        case AST::OperationType::BINOP_MINUS: {
            return builder->CreateFSub(l, r);
        }
        case AST::OperationType::BINOP_ADD: {
            return builder->CreateFAdd(l, r);
        }
        case AST::OperationType::BINOP_MULTIPLY: {
            return builder->CreateFMul(l, r);
        }
        case AST::OperationType::BINOP_DIVIDE: {
            return builder->CreateFDiv(l, r);
        }
        default: {
            logError("unknown binary operation");
            return nullptr;
        }
    }
}

llvm::Value *CodeGenerator::codegenCall(const std::unique_ptr<AST::Call> &call) {
    auto *callee = module->getFunction(call->getCallee());
    if (callee == nullptr) {
        logError("unknown function call");
        return nullptr;
    }
    if (callee->arg_size() != call->getArgs().size()) {
        logError("incorrect numberDouble of arguments");
        return nullptr;
    }
    std::vector<llvm::Value *> args;
    for (const auto &item: call->getArgs()) {
        args.push_back(codegenExpression(item));
    }
    return builder->CreateCall(callee, args);
}

llvm::Function *CodeGenerator::codegenPrototype(const std::unique_ptr<AST::Prototype> &prototype) {
    std::vector<llvm::Type *> doubles(prototype->getArgs().size(), llvm::Type::getDoubleTy(*context));
    auto *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), doubles, false);
    auto *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, prototype->getName(), *module);
    int i = 0;
    for (auto &item: f->args()) {
        item.setName(prototype->getArgs()[i]);
        i++;
    }
    return f;
}

llvm::Function *CodeGenerator::codegenFunctionDef(const std::unique_ptr<AST::FunctionDef> &functionDef) {
    auto *f = module->getFunction(functionDef->getPrototype()->getName());
    if (f == nullptr) {
        f = codegenPrototype(functionDef->getPrototype());
    }
    if (f == nullptr) {
        return nullptr;
    }
    if (!f->empty()) {
        logError("function redefinition");
        return nullptr;
    }
    auto *bb = llvm::BasicBlock::Create(*context, "", f);
    builder->SetInsertPoint(bb);
    scope.clear();
    for (auto &item: f->args()) {
        scope[std::string(item.getName())] = &item;
    }
    auto *retVal = codegenExpression(functionDef->getBody());
    if (retVal != nullptr) {
        builder->CreateRet(retVal);
        llvm::verifyFunction(*f);
        fpm->run(*f);
        return f;
    }
    f->eraseFromParent();
    return nullptr;
}

void CodeGenerator::initPassManager() {
    fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module.get());
    fpm->add(llvm::createInstructionCombiningPass());
    fpm->add(llvm::createReassociatePass());
    fpm->add(llvm::createGVNPass());
    fpm->add(llvm::createCFGSimplificationPass());
    fpm->doInitialization();
}

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("ultra-ruby", *context);

    initPassManager();
}
} // UltraRuby
} // IR*/
