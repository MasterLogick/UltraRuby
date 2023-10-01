#include <iostream>
#include "CodeGenerator.h"
#include "../lang/Symbol.h"
#include "../lang/String.h"
#include "../ast/AST.h"

namespace UltraRuby {
namespace IR {
constexpr bool voidpIsUint64 = sizeof(void *) == sizeof(uint64_t);

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("ultra-ruby", *context);
    declareExternLangFunctions();
}

void CodeGenerator::codegenProgram(const std::unique_ptr<AST::FunctionDef> &topLevelDecl) {
    scope = new Scope();
    declareFunction(topLevelDecl);
    codegenFunctionBody(topLevelDecl);
}

llvm::Value *CodeGenerator::codegenStatement(const std::unique_ptr<AST::Statement> &statement) {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), llvm::APInt(32, 5));
}

llvm::Function *CodeGenerator::codegenFunctionBody(const std::unique_ptr<AST::FunctionDef> &functionDef) {
    auto *func = module->getFunction(scope->deriveFunctionName(functionDef));
    if (func == nullptr) {
        logError("undeclared function");
        return nullptr;
    }
    if (!func->empty()) {
        logError("function redefinition");
        return nullptr;
    }
    scope = scope->enterFunctionBody(functionDef);
    auto *bb = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(bb);
    if (!codegenArgsProcessingPreamble(functionDef)) {
        func->eraseFromParent();
        return nullptr;
    }
    auto *retVal = codegenStatement(functionDef->getBody());
    if (retVal == nullptr) {
        func->eraseFromParent();
        return nullptr;
    }
    builder->CreateRet(retVal);
    // todo verify using llvm::verifyFunction(*func);
    return func;
}

void CodeGenerator::declareFunction(const std::unique_ptr<AST::FunctionDef> &functionDef) {
    auto &args = functionDef->getArgs();
    std::vector<llvm::Type *> argTypes(args->getMaxArgsCount(), llvm::PointerType::getUnqual(*context));
    auto *ft = llvm::FunctionType::get(llvm::PointerType::getUnqual(*context), argTypes, false);
    auto *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, scope->deriveFunctionName(functionDef),
                                     *module);
    //todo set arg names
    /*int i = 0;
    for (auto &item: f->args()) {
        item.setName(prototype->getArgs()[i]);
        i++;
    }*/
}

llvm::Constant *CodeGenerator::codegenSymbol(const std::unique_ptr<AST::Symbol> &symbol) {
    auto *s = Lang::Symbol::get(symbol->getSym());
    return codegenPointer(s);
}

llvm::Constant *CodeGenerator::codegenIntegerConst(const std::unique_ptr<AST::IntegerConst> &intConst) {
    //todo handle cases if val is bigint or is in hex/octal/bin notation
    if (voidpIsUint64) {
        int v = std::stoi(intConst->getVal());
        return llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*context), llvm::APInt(32, v));
    } else {
        assert(false);
    }
}

llvm::Constant *CodeGenerator::codegenFloatConst(const std::unique_ptr<AST::FloatConst> &floatConst) {
    if (voidpIsUint64) {
        float v = std::stof(floatConst->getVal());
        return llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), v);
    } else {
        assert(false);
    }
}

llvm::Constant *CodeGenerator::codegenString(const std::unique_ptr<AST::String> &str) {
    auto *s = Lang::String::get(str->getStr());
    return codegenPointer(s);
}

llvm::Constant *CodeGenerator::codegenPointer(void *data) {
    if (voidpIsUint64) {
        return llvm::Constant::getIntegerValue(llvm::PointerType::getUnqual(*context),
                                               llvm::APInt(64, reinterpret_cast<uint64_t>(data)));
    } else {
        //todo handle 32-bit cases
        assert(false);
    }
}

llvm::Value *CodeGenerator::codegenArray(const std::unique_ptr<AST::Array> &array) {
    std::vector<llvm::Value *> values(array->getElements().size() + 1);
    values.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context), llvm::APInt(32, array->getElements().size())));
    for (const auto &item: array->getElements()) {
        values.push_back(codegenStatement(item));
    }
    return builder->CreateCall(langArrayAlloc->getFunctionType(), langArrayAlloc, values, "array");
}

llvm::Value *CodeGenerator::codegenHash(const std::unique_ptr<AST::Hash> &hash) {
    std::vector<llvm::Value *> values(hash->getHash().size() * 2 + 1);
    values.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context), llvm::APInt(32, hash->getHash().size())));
    for (const auto &item: hash->getHash()) {
        auto *key = codegenStatement(item.first);
        values.push_back(key);
        auto *val = codegenStatement(item.second);
        values.push_back(val);
    }
    return builder->CreateCall(langHashAlloc->getFunctionType(), langHashAlloc, values, "hash");
}

llvm::Value *CodeGenerator::codegenAssignment(const std::unique_ptr<AST::Assignment> &assignment) {
    llvm::Value *dst = scope->getVariable(assignment->getDst());
    auto *src = codegenStatement(assignment->getSrc());
    if (src == nullptr) {
        return nullptr;
    }
    builder->CreateStore(src, dst);
    return src;
}

llvm::Value *CodeGenerator::codegenBinaryOperation(const std::unique_ptr<AST::BinaryOperation> &binOp) {
    std::string opLabel;
    switch (binOp->getOperator()) {
        case AST::BIN_OP_LESS:
            opLabel = "<";
            break;
        case AST::BIN_OP_LESS_EQUAL:
            opLabel = "<=";
            break;
        case AST::BIN_OP_EQUAL:
            opLabel = "==";
            break;
        case AST::BIN_OP_GREATER_EQUAL:
            opLabel = ">=";
            break;
        case AST::BIN_OP_GREATER:
            opLabel = ">";
            break;
        case AST::BIN_OP_THREE_WAY_COMPARE:
            opLabel = "<=>";
            break;

        case AST::BIN_OP_BIN_OR:
        case AST::BIN_OP_BIN_OR_ASSIGN:
            opLabel = "|";
            break;
        case AST::BIN_OP_OR:
        case AST::BIN_OP_OR_ASSIGN:
            opLabel = "||";
            break;
        case AST::BIN_OP_BIN_AND:
        case AST::BIN_OP_BIN_AND_ASSIGN:
            opLabel = "&";
            break;
        case AST::BIN_OP_AND:
        case AST::BIN_OP_AND_ASSIGN:
            opLabel = "&";
            break;
        case AST::BIN_OP_XOR:
        case AST::BIN_OP_XOR_ASSIGN:
            opLabel = "^";
            break;
        case AST::BIN_OP_LEFT_SHIFT:
        case AST::BIN_OP_LEFT_SHIFT_ASSIGN:
            opLabel = "<<";
            break;
        case AST::BIN_OP_RIGHT_SHIFT:
        case AST::BIN_OP_RIGHT_SHIFT_ASSIGN:
            opLabel = ">>";
            break;
        case AST::BIN_OP_ADD:
        case AST::BIN_OP_ADD_ASSIGN:
            opLabel = "+";
            break;
        case AST::BIN_OP_SUBTRACT:
        case AST::BIN_OP_SUBTRACT_ASSIGN:
            opLabel = "-";
            break;
        case AST::BIN_OP_MULTIPLY:
        case AST::BIN_OP_MULTIPLY_ASSIGN:
            opLabel = "*";
            break;
        case AST::BIN_OP_DIVIDE:
        case AST::BIN_OP_DIVIDE_ASSIGN:
            opLabel = "/";
            break;
        case AST::BIN_OP_MOD:
        case AST::BIN_OP_MOD_ASSIGN:
            opLabel = "%";
            break;
        case AST::BIN_OP_POWER:
        case AST::BIN_OP_POWER_ASSIGN:
            opLabel = "**";
            break;
        default:
            logError("not a binary operation");
            return nullptr;
    }
    if (binOp->getOperator() < AST::BIN_OP_ASSIGNMENT_START) {
        return codegenCall(codegenStatement(binOp->getLeft()), codegenSymbol(std::make_unique<AST::Symbol>(opLabel)),
                           std::vector<llvm::Value *>{codegenStatement(binOp->getRight())});
    } else {
        //todo
        return nullptr;
    }
}

void CodeGenerator::declareExternLangFunctions() {
    std::vector<llvm::Type *> argTypes;
    auto *voidp = llvm::PointerType::getUnqual(*context);
    auto *int32Ty = llvm::Type::getInt32Ty(*context);
    auto *voidp_IntVariadic = llvm::FunctionType::get(
            voidp,
            std::vector<llvm::Type *>{int32Ty},
            true);
    auto *voidp_2voidp = llvm::FunctionType::get(llvm::PointerType::getUnqual(*context),
                                                 std::vector<llvm::Type *>(2, voidp), false);
    auto *voidp_3voidp = llvm::FunctionType::get(llvm::PointerType::getUnqual(*context),
                                                 std::vector<llvm::Type *>(3, voidp), false);
    auto *voidp_4voidp = llvm::FunctionType::get(llvm::PointerType::getUnqual(*context),
                                                 std::vector<llvm::Type *>(4, voidp), false);
    auto *voidp_2voidp2intvoidp = llvm::FunctionType::get(
            llvm::PointerType::getUnqual(*context),
            std::vector<llvm::Type *>{voidp, voidp, int32Ty, int32Ty, voidp},
            false);

    langArrayAlloc = llvm::Function::Create(voidp_IntVariadic, llvm::Function::ExternalLinkage,
                                            "_ZN9UltraRuby4Lang5Array5allocEiz", *module);
    langHashAlloc = llvm::Function::Create(voidp_IntVariadic, llvm::Function::ExternalLinkage,
                                           "_ZN9UltraRuby4Lang4Hash5allocEiz", *module);
    langObjectCall0 = llvm::Function::Create(voidp_2voidp, llvm::Function::ExternalLinkage,
                                             "_ZN9UltraRuby4Lang6Object5call0EPNS0_6SymbolE", *module);
    langObjectCall1 = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                             "_ZN9UltraRuby4Lang6Object5call1EPNS0_6SymbolEPS1_", *module);
    langObjectCall2 = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                             "_ZN9UltraRuby4Lang6Object5call2EPNS0_6SymbolEPS1_S4_", *module);
    langObjectCallV = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                             "_ZN9UltraRuby4Lang6Object5callVEPNS0_6SymbolEPS1_", *module);
    langObjectDefineInstanceMethod = llvm::Function::Create(voidp_2voidp2intvoidp, llvm::Function::ExternalLinkage,
                                                            "_ZN9UltraRuby4Lang6Object20defineInstanceMethodEPNS0_6SymbolEiiPv",
                                                            *module);
    langObjectDefineSingletonMethod = llvm::Function::Create(voidp_2voidp2intvoidp, llvm::Function::ExternalLinkage,
                                                             "_ZN9UltraRuby4Lang6Object21defineSingletonMethodEPNS0_6SymbolEiiPv",
                                                             *module);
    langClassDefineClass = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                  "_ZN9UltraRuby4Lang5Class11defineClassEPPS1_PNS0_6SymbolEPFPNS0_6ObjectES2_E",
                                                  *module);
}

void CodeGenerator::debugPrintModuleIR() {
    module->dump();
}

bool CodeGenerator::codegenArgsProcessingPreamble(const std::unique_ptr<AST::FunctionDef> &functionDef) {
    return true;
}

void CodeGenerator::logError(const char *error) {
    std::cerr << "Code generator error: " << error << std::endl;
}

void CodeGenerator::setTarget(llvm::TargetMachine *machine, std::string triple) {
    module->setDataLayout(machine->createDataLayout());
    module->setTargetTriple(triple);
}

void CodeGenerator::runPass(llvm::legacy::PassManager &manager) {
    manager.run(*module);
}

llvm::Constant *CodeGenerator::codegenBoolConst(bool val) {
    if (voidpIsUint64) {
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), llvm::APInt(1, val));
    } else {
        assert(false);
    }
}
}
}
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
