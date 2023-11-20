#include <iostream>
#include "CodeGenerator.h"
#include "../lang/Symbol.h"
#include "../lang/String.h"
#include "../ast/AST.h"
#include "../lang/Exception.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>

namespace UltraRuby {
namespace IR {
constexpr bool voidpIsUint64 = sizeof(void *) == sizeof(uint64_t);

CodeGenerator::CodeGenerator() {
    scope = new Scope();
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("ultra-ruby", *context);
    declareExternLangFunctions();
}

llvm::Function *CodeGenerator::codegenProgram(AST::FunctionDef *topLevelDecl) {
    auto *v = codegenFunctionInternal(topLevelDecl);
    v->setLinkage(llvm::GlobalValue::ExternalLinkage);
    return v;
}

llvm::Value *CodeGenerator::codegenStatement(AST::Statement *statement) {
    assert(statement != nullptr);
    switch (statement->type) {
        case AST::STMT_BIN_OP:
            return codegenBinaryOperation(reinterpret_cast<AST::BinaryOperation *>(statement));
        case AST::STMT_UN_OP:
            return codegenUnaryOperation(reinterpret_cast<AST::UnaryOperation *>(statement));
        case AST::STMT_CALL:
            return codegenCall(reinterpret_cast<AST::Call *>(statement));
        case AST::STMT_FLOAT_CONST:
            return codegenFloatConst(reinterpret_cast<AST::FloatConst *>(statement));
        case AST::STMT_INT_CONST:
            return codegenIntegerConst(reinterpret_cast<AST::IntegerConst *>(statement));
        case AST::STMT_STRING:
            return codegenString(reinterpret_cast<AST::String *>(statement));
        case AST::STMT_SYMBOL:
            return codegenSymbol(reinterpret_cast<AST::Symbol *>(statement));
        case AST::STMT_ARRAY:
            return codegenArray(reinterpret_cast<AST::Array *>(statement));
        case AST::STMT_HASH:
            return codegenHash(reinterpret_cast<AST::Hash *>(statement));
        case AST::STMT_LOCAL_VARIABLE:
            return codegenLocalVariable(reinterpret_cast<AST::LocalVariable *>(statement));
        case AST::STMT_CLASS_DEF:
            return codegenClassDef(reinterpret_cast<AST::ClassDef *>(statement));
        case AST::STMT_MODULE_DEF:
            return codegenModuleDef(reinterpret_cast<AST::ModuleDef *>(statement));
        case AST::STMT_FUNC_DEF:
            return codegenFunctionDef(reinterpret_cast<AST::FunctionDef *>(statement));
        case AST::STMT_CLASS_INSTANCE_DEF:
            return codegenClassInstanceDef(reinterpret_cast<AST::ClassInstanceDef *>(statement));
        case AST::STMT_BLOCK:
            return codegenBlock(reinterpret_cast<AST::Block *>(statement));
        case AST::STMT_EXCEPTIONAL_BLOCK:
            return codegenExceptionalBlock(reinterpret_cast<AST::ExceptionalBlock *>(statement));
        case AST::STMT_IF:
            return codegenIf(reinterpret_cast<AST::If *>(statement));
        case AST::STMT_WHILE:
            return codegenWhile(reinterpret_cast<AST::While *>(statement));
        case AST::STMT_CASE:
            return codegenCase(reinterpret_cast<AST::Case *>(statement));
        case AST::STMT_FOR:
            return codegenFor(reinterpret_cast<AST::For *>(statement));
        case AST::STMT_BOOL_CONST:
            return codegenBoolConst(reinterpret_cast<AST::BoolConst *>(statement)->getVal());
        case AST::STMT_RETURN:
            return codegenReturn(reinterpret_cast<AST::Return *>(statement));
        case AST::STMT_NEXT:
            return codegenNext(reinterpret_cast<AST::Next *>(statement));
        case AST::STMT_BREAK:
            return codegenBreak(reinterpret_cast<AST::Break *>(statement));
        case AST::STMT_REDO:
            return codegenBinaryOperation(reinterpret_cast<AST::BinaryOperation *>(statement));
        case AST::STMT_RETRY:
            return codegenRetry();
        case AST::STMT_YIELD:
            return codegenYield(reinterpret_cast<AST::Yield *>(statement));
        case AST::STMT_YIELD_SELF:
            return codegenYieldSelf();
        case AST::STMT_INSTANCE_VARIABLE:
            return codegenInstanceVariable(reinterpret_cast<AST::InstanceVariable *>(statement));
        case AST::STMT_UNKNOWN:
            return nullptr;
        case AST::STMT_LANG_VARIABLE:
            return codegenLangVariable(reinterpret_cast<AST::LangVariable *>(statement));
        case AST::STMT_CONSTANT_REF:
            return codegenConstantRef(reinterpret_cast<AST::ConstantRef *>(statement));
    }
}

llvm::Function *CodeGenerator::codegenFunctionInternal(AST::FunctionDef *functionDef) {
    auto ip = builder->saveIP();
    auto *func = module->getFunction(functionDef->getName());
    if (func != nullptr) {
        logError("function redeclaration");
        return nullptr;
    }
    auto args = functionDef->getArgs();
    std::vector<llvm::Type *> argTypes(functionDef->getMaxArgsCount() + 1, voidpTy);
    auto *ft = llvm::FunctionType::get(voidpTy, argTypes, false);
    func = llvm::Function::Create(ft, llvm::Function::PrivateLinkage, functionDef->getName(), *module);
    auto *bb = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(bb);
    if (!codegenArgsProcessingPreamble(functionDef, func)) {
        func->eraseFromParent();
        return nullptr;
    }
    auto *retVal = codegenStatement(functionDef->getBody());
    if (retVal == nullptr) {
        func->eraseFromParent();
        return nullptr;
    }
    builder->CreateRet(retVal);
    llvm::verifyFunction(*func);
    builder->restoreIP(ip);
    return func;
}

llvm::LoadInst *CodeGenerator::codegenSymbol(AST::Symbol *symbol) {
    return codegenSymbol(symbol->getSym());
}

llvm::LoadInst *CodeGenerator::codegenSymbol(const std::string &symbol) {
    auto name = "sym:" + symbol;
    if (auto *l = module->getGlobalVariable(name, true)) {
        return builder->CreateLoad(voidpTy, l);
    }
    auto *variable = new llvm::GlobalVariable(*module, voidpTy, false, llvm::GlobalValue::PrivateLinkage,
                                              llvm::ConstantPointerNull::get(voidpTy), name);
    syms.emplace(symbol, variable);
    return builder->CreateLoad(voidpTy, variable);
}

llvm::Value *CodeGenerator::codegenIntegerConst(AST::IntegerConst *intConst) {
    //todo handle cases if val is bigint or is in hex/octal/bin notation
    if (voidpIsUint64) {
        int v = std::stoi(intConst->getVal());
        auto *constVal = builder->getInt64(v);
        return builder->CreateIntToPtr(constVal, voidpTy);
    } else {
        assert(false);
    }
}

llvm::Value *CodeGenerator::codegenFloatConst(AST::FloatConst *floatConst) {
    //todo check machine float size
    if (voidpIsUint64) {
        float v = std::stof(floatConst->getVal());
        return builder->CreateIntToPtr(llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), v), voidpTy);
    } else {
        assert(false);
    }
}

llvm::LoadInst *CodeGenerator::codegenString(AST::String *str) {
    auto name = "str:" + str->getStr();
    if (auto *l = module->getGlobalVariable(name, true)) {
        return builder->CreateLoad(voidpTy, l);
    }
    auto *variable = new llvm::GlobalVariable(*module, voidpTy, false, llvm::GlobalValue::PrivateLinkage,
                                              llvm::ConstantPointerNull::get(voidpTy), name);
    strs.emplace(str->getStr(), variable);
    return builder->CreateLoad(voidpTy, variable);
}

llvm::Constant *CodeGenerator::codegenPointer(void *data) {
    if (voidpIsUint64) {
        return llvm::Constant::getIntegerValue(voidpTy, llvm::APInt(64, reinterpret_cast<uint64_t>(data)));
    } else {
        //todo handle 32-bit cases
        assert(false);
    }
}

llvm::Constant *CodeGenerator::codegenConstPointer(const void *data) {
    if (voidpIsUint64) {
        return llvm::Constant::getIntegerValue(voidpTy, llvm::APInt(64, reinterpret_cast<uint64_t>(data)));
    } else {
        //todo handle 32-bit cases
        assert(false);
    }
}

llvm::Value *CodeGenerator::codegenArray(AST::Array *array) {
    auto elems = array->getElements();
    if (elems.empty()) {
        return codegenLangCall(langArrayAlloc, {builder->getInt32(0), codegenPointer(nullptr)});
    }
    auto *arrAlloca = builder->CreateAlloca(voidpTy, builder->getInt32(elems.size()), "arrayTmp");
    for (int i = 0; i < elems.size(); ++i) {
        auto *el = codegenStatement(elems[i]);
        if (el == nullptr) {
            return nullptr;
        }
        builder->CreateStore(el, builder->CreateGEP(voidpTy, arrAlloca, {builder->getInt32(i)}));
    }
    return codegenLangCall(langArrayAlloc, {builder->getInt32(elems.size()), arrAlloca});
}

llvm::Value *CodeGenerator::codegenHash(AST::Hash *hash) {
    auto pairs = hash->getHash();
    if (pairs.empty()) {
        return codegenLangCall(langHashAlloc, {builder->getInt32(0), codegenPointer(nullptr)});
    }
    auto *arrAlloca = builder->CreateAlloca(voidpTy, builder->getInt32(pairs.size() * 2), "hashTmp");
    for (int i = 0; i < pairs.size(); ++i) {
        auto *k = codegenStatement(pairs[i].first);
        if (k == nullptr) {
            return nullptr;
        }
        auto *v = codegenStatement(pairs[i].second);
        builder->CreateStore(k, builder->CreateGEP(voidpTy, arrAlloca, {builder->getInt32(i * 2)}));
        builder->CreateStore(v, builder->CreateGEP(voidpTy, arrAlloca, {builder->getInt32(i * 2 + 1)}));
    }
    return codegenLangCall(langHashAlloc, {builder->getInt32(pairs.size()), arrAlloca});
}

const std::map<AST::OperationType, std::string> ops = {
        {AST::BIN_OP_LESS,               "<"},
        {AST::BIN_OP_LESS_EQUAL,         "<="},
        {AST::BIN_OP_EQUAL,              "=="},
        {AST::BIN_OP_CASE_EQUAL,         "==="},
        {AST::BIN_OP_GREATER_EQUAL,      ">="},
        {AST::BIN_OP_GREATER,            ">"},
        {AST::BIN_OP_THREE_WAY_COMPARE,  "<=>"},
        {AST::BIN_OP_BIN_OR,             "|"},
        {AST::BIN_OP_OR,                 "||"},
        {AST::BIN_OP_BIN_AND,            "&"},
        {AST::BIN_OP_AND,                "&&"},
        {AST::BIN_OP_XOR,                "^"},
        {AST::BIN_OP_LEFT_SHIFT,         "<<"},
        {AST::BIN_OP_RIGHT_SHIFT,        ">>"},
        {AST::LEX_OP_PLUS,               "+"},
        {AST::LEX_OP_MINUS,              "-"},
        {AST::LEX_OP_STAR,               "*"},
        {AST::BIN_OP_DIVIDE,             "/"},
        {AST::BIN_OP_MOD,                "%"},
        {AST::BIN_OP_POWER,              "**"},
        {AST::BIN_OP_ASSIGN,             ""},
        {AST::BIN_OP_BIN_OR_ASSIGN,      "|"},
        {AST::BIN_OP_OR_ASSIGN,          "||"},
        {AST::BIN_OP_BIN_AND_ASSIGN,     "&"},
        {AST::BIN_OP_AND_ASSIGN,         "&&"},
        {AST::BIN_OP_XOR_ASSIGN,         "^"},
        {AST::BIN_OP_LEFT_SHIFT_ASSIGN,  "<<"},
        {AST::BIN_OP_RIGHT_SHIFT_ASSIGN, ">>"},
        {AST::BIN_OP_ADD_ASSIGN,         "+"},
        {AST::BIN_OP_SUBTRACT_ASSIGN,    "-"},
        {AST::BIN_OP_MULTIPLY_ASSIGN,    "*"},
        {AST::BIN_OP_DIVIDE_ASSIGN,      "/"},
        {AST::BIN_OP_MOD_ASSIGN,         "%"},
        {AST::BIN_OP_POWER_ASSIGN,       "**"},
};

llvm::Value *CodeGenerator::codegenBinaryOperation(AST::BinaryOperation *binOp) {
    if (!ops.contains(binOp->getOperator())) {
        logError("unexpected binary operator");
        return nullptr;
    }
    std::string opLabel = ops.at(binOp->getOperator());
    if (AST::BIN_OP_REGION_START <= binOp->getOperator() && binOp->getOperator() < AST::BIN_OP_ASSIGNMENT_START) {
        AST::CallArgs args({binOp->getRight()}, {}, nullptr, false, true);
        AST::Call call(opLabel, &args, binOp->getLeft());
        return codegenLangCall(langObjectCall[1], {codegenStatement(binOp->getLeft()), codegenSymbol(opLabel),
                                                   codegenStatement(binOp->getRight())});
    } else {
        assert(AST::BIN_OP_ASSIGNMENT_START <= binOp->getOperator() && binOp->getOperator() < AST::BIN_OP_QUESTION &&
               "unknown binary operation");
        auto *left = binOp->getLeft();
        switch (left->type) {
            case AST::STMT_LOCAL_VARIABLE: {
                auto *lv = reinterpret_cast<AST::LocalVariable *>(left);
                auto *alloca = scope->getLocalVariable(lv->getName());
                if (alloca == nullptr) {
                    alloca = builder->CreateAlloca(voidpTy, nullptr, lv->getName() + "_alloca");
                    scope->addLocalVariable(lv->getName(), alloca);
                }
                auto *rval = codegenStatement(binOp->getRight());
                if (rval == nullptr)return nullptr;
                builder->CreateStore(rval, alloca);
                return rval;
            }
            case AST::STMT_CALL: {
                auto *call = reinterpret_cast<AST::Call *>(left);
                auto *callArgs = call->getArgs();
                if ((callArgs->hasParenthesis() && !call->getName().empty()) || callArgs->getBlock() ||
                    (!callArgs->hasBrackets() && (!callArgs->getArgs().empty()))) {
                    logError("unexpected operator-assignment");
                    return nullptr;
                }
                if (!callArgs->getNamedArgs().empty()) {
                    //todo
                    logError("unimplemented");
                    return nullptr;
                }
                auto *lhsContainer = call->getObject();
                auto *lhsContainerVal = lhsContainer != nullptr ? codegenStatement(lhsContainer) : codegenSelf();

                llvm::Value *rhsValue;
                std::string setValMethodName;

                if (callArgs->hasBrackets()) {
                    std::vector<llvm::Value *> bracketArgs;
                    for (const auto &arg: callArgs->getArgs()) {
                        auto *v = codegenStatement(arg);
                        if (v == nullptr) {
                            return nullptr;
                        }
                        bracketArgs.push_back(v);
                    }
                    auto *lhsValBefore = codegenCall(lhsContainerVal, codegenSymbol("[]"),
                                                     bracketArgs.size(), bracketArgs);
                    auto *rhs = codegenLangCall(langObjectCall[1], {lhsValBefore, codegenSymbol(opLabel),
                                                                    codegenStatement(binOp->getRight())});
                    bracketArgs.push_back(rhs);
                    return codegenCall(lhsContainerVal, codegenSymbol("[]="), bracketArgs.size(), bracketArgs);
                } else {
                    AST::CallArgs setCallArgs({binOp->getRight()}, {}, nullptr, false, true);
                    AST::Call setCall(call->getName() + "=", &setCallArgs, nullptr);
                    return codegenCall(lhsContainerVal, &setCall);
                }
            }
            default: {
                logError("unexpected left hand side");
                return nullptr;
            }
        }
    }
}

void CodeGenerator::declareExternLangFunctions() {
    voidpTy = llvm::PointerType::getUnqual(*context);
    auto *int32Ty = llvm::Type::getInt32Ty(*context);
    auto *voidp_intvoidp = llvm::FunctionType::get(voidpTy, {int32Ty, voidpTy}, false);

    auto *void_ = llvm::FunctionType::get(builder->getVoidTy(), {}, false);
    auto *voidp_1voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(1, voidpTy), false);
    auto *voidp_2voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(2, voidpTy), false);
    auto *voidp_3voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(3, voidpTy), false);
    auto *voidp_4voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(4, voidpTy), false);
    auto *voidp_5voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(5, voidpTy), false);
    auto *voidp_6voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(6, voidpTy), false);
    auto *voidp_7voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(7, voidpTy), false);
    auto *voidp_8voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(8, voidpTy), false);
    auto *voidp_9voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(9, voidpTy), false);
    auto *voidp_2voidpintvoidp = llvm::FunctionType::get(voidpTy, {voidpTy, voidpTy, int32Ty, voidpTy}, false);
    auto *voidp_3voidpintvoidp = llvm::FunctionType::get(voidpTy, {voidpTy, voidpTy, voidpTy, int32Ty, voidpTy}, false);
    auto *voidp_4voidpintvoidp = llvm::FunctionType::get(voidpTy,
                                                         {voidpTy, voidpTy, voidpTy, voidpTy, int32Ty, voidpTy}, false);

    functionDefMetaType = llvm::StructType::create(*context, "FunctionDefMeta");
    int8Ty = llvm::Type::getInt8Ty(*context);
    functionDefMetaType->setBody({voidpTy, int8Ty, int8Ty, int8Ty, int8Ty, int8Ty, int8Ty}, true);

    exceptionHandlerType = llvm::StructType::create(*context, "ExceptionHandler");
    exceptionHandlerType->setBody({voidpTy, int32Ty}, true);

    langArrayAlloc = llvm::Function::Create(voidp_intvoidp, llvm::Function::ExternalLinkage,
                                            "_ZN9UltraRuby4Lang5Array11allocOnHeapEiPPNS0_6ObjectE", *module);
    langHashAlloc = llvm::Function::Create(voidp_intvoidp, llvm::Function::ExternalLinkage,
                                           "_ZN9UltraRuby4Lang4Hash11allocOnHeapEiPPNS0_6ObjectE", *module);

    langObjectCall[0] = llvm::Function::Create(voidp_2voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object4callIJEEEPS1_PNS0_6SymbolEDpT_", *module);
    langObjectCall[1] = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object4callIJPS1_EEES3_PNS0_6SymbolEDpT_", *module);
    langObjectCall[2] = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object4callIJPS1_S3_EEES3_PNS0_6SymbolEDpT_",
                                               *module);
    langObjectCall[3] = llvm::Function::Create(voidp_5voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object4callIJPS1_S3_S3_EEES3_PNS0_6SymbolEDpT_",
                                               *module);
    langObjectCall[4] = llvm::Function::Create(voidp_6voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object4callIJPS1_S3_S3_S3_EEES3_PNS0_6SymbolEDpT_",
                                               *module);
    langObjectCall[5] = llvm::Function::Create(voidp_7voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object4callIJPS1_S3_S3_S3_S3_EEES3_PNS0_6SymbolEDpT_",
                                               *module);

    langObjectCallB[0] = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callBIJEEEPS1_PNS0_6SymbolEPNS0_4ProcEDpT_",
                                                *module);
    langObjectCallB[1] = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callBIJPS1_EEES3_PNS0_6SymbolEPNS0_4ProcEDpT_",
                                                *module);
    langObjectCallB[2] = llvm::Function::Create(voidp_5voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callBIJPS1_S3_EEES3_PNS0_6SymbolEPNS0_4ProcEDpT_",
                                                *module);
    langObjectCallB[3] = llvm::Function::Create(voidp_6voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callBIJPS1_S3_S3_EEES3_PNS0_6SymbolEPNS0_4ProcEDpT_",
                                                *module);
    langObjectCallB[4] = llvm::Function::Create(voidp_7voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callBIJPS1_S3_S3_S3_EEES3_PNS0_6SymbolEPNS0_4ProcEDpT_",
                                                *module);
    langObjectCallB[5] = llvm::Function::Create(voidp_8voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callBIJPS1_S3_S3_S3_S3_EEES3_PNS0_6SymbolEPNS0_4ProcEDpT_",
                                                *module);

    langObjectCallN[0] = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callNIJEEEPS1_PNS0_6SymbolEPNS0_4HashEDpT_",
                                                *module);
    langObjectCallN[1] = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callNIJPS1_EEES3_PNS0_6SymbolEPNS0_4HashEDpT_",
                                                *module);
    langObjectCallN[2] = llvm::Function::Create(voidp_5voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callNIJPS1_S3_EEES3_PNS0_6SymbolEPNS0_4HashEDpT_",
                                                *module);
    langObjectCallN[3] = llvm::Function::Create(voidp_6voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callNIJPS1_S3_S3_EEES3_PNS0_6SymbolEPNS0_4HashEDpT_",
                                                *module);
    langObjectCallN[4] = llvm::Function::Create(voidp_7voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callNIJPS1_S3_S3_S3_EEES3_PNS0_6SymbolEPNS0_4HashEDpT_",
                                                *module);
    langObjectCallN[5] = llvm::Function::Create(voidp_8voidp, llvm::Function::ExternalLinkage,
                                                "_ZN9UltraRuby4Lang6Object5callNIJPS1_S3_S3_S3_S3_EEES3_PNS0_6SymbolEPNS0_4HashEDpT_",
                                                *module);

    langObjectCallNB[0] = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                                 "_ZN9UltraRuby4Lang6Object6callNBIJEEEPS1_PNS0_6SymbolEPNS0_4HashEPNS0_4ProcEDpT_",
                                                 *module);
    langObjectCallNB[1] = llvm::Function::Create(voidp_5voidp, llvm::Function::ExternalLinkage,
                                                 "_ZN9UltraRuby4Lang6Object6callNBIJPS1_EEES3_PNS0_6SymbolEPNS0_4HashEPNS0_4ProcEDpT_",
                                                 *module);
    langObjectCallNB[2] = llvm::Function::Create(voidp_6voidp, llvm::Function::ExternalLinkage,
                                                 "_ZN9UltraRuby4Lang6Object6callNBIJPS1_S3_EEES3_PNS0_6SymbolEPNS0_4HashEPNS0_4ProcEDpT_",
                                                 *module);
    langObjectCallNB[3] = llvm::Function::Create(voidp_7voidp, llvm::Function::ExternalLinkage,
                                                 "_ZN9UltraRuby4Lang6Object6callNBIJPS1_S3_S3_EEES3_PNS0_6SymbolEPNS0_4HashEPNS0_4ProcEDpT_",
                                                 *module);
    langObjectCallNB[4] = llvm::Function::Create(voidp_8voidp, llvm::Function::ExternalLinkage,
                                                 "_ZN9UltraRuby4Lang6Object6callNBIJPS1_S3_S3_S3_EEES3_PNS0_6SymbolEPNS0_4HashEPNS0_4ProcEDpT_",
                                                 *module);
    langObjectCallNB[5] = llvm::Function::Create(voidp_9voidp, llvm::Function::ExternalLinkage,
                                                 "_ZN9UltraRuby4Lang6Object6callNBIJPS1_S3_S3_S3_S3_EEES3_PNS0_6SymbolEPNS0_4HashEPNS0_4ProcEDpT_",
                                                 *module);

    langObjectCallV = llvm::Function::Create(voidp_2voidpintvoidp, llvm::Function::ExternalLinkage,
                                             "_ZN9UltraRuby4Lang6Object5callVEPNS0_6SymbolEiPPS1_", *module);
    langObjectCallBV = llvm::Function::Create(voidp_3voidpintvoidp, llvm::Function::ExternalLinkage,
                                              "_ZN9UltraRuby4Lang6Object6callBVEPNS0_6SymbolEPNS0_4ProcEiPPS1_",
                                              *module);
    langObjectCallNV = llvm::Function::Create(voidp_3voidpintvoidp, llvm::Function::ExternalLinkage,
                                              "_ZN9UltraRuby4Lang6Object6callNVEPNS0_6SymbolEPNS0_4HashEiPPS1_",
                                              *module);
    langObjectCallNBV = llvm::Function::Create(voidp_4voidpintvoidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Object7callNBVEPNS0_6SymbolEPNS0_4HashEPNS0_4ProcEiPPS1_",
                                               *module);

    langObjectDefineInstanceMethod = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                            "_ZN9UltraRuby4Lang6Object20defineInstanceMethodEPNS0_6SymbolEPNS0_15FunctionDefMetaE",
                                                            *module);
    langObjectDefineSingletonMethod = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                             "_ZN9UltraRuby4Lang6Object21defineSingletonMethodEPNS0_6SymbolEiiPv",
                                                             *module);
    langObjectDefineClassInstance = llvm::Function::Create(voidp_2voidp, llvm::Function::ExternalLinkage,
                                                           "_ZN9UltraRuby4Lang6Object19defineClassInstanceEPFPS1_S2_E",
                                                           *module);
    langObjectDefineClass = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                                   "_ZN9UltraRuby4Lang6Object11defineClassEPNS0_6SymbolEPNS0_5ClassEPFPS1_S5_E",
                                                   *module);
    langObjectDefineModule = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                    "_ZN9UltraRuby4Lang6Object12defineModuleEPNS0_6SymbolEPFPS1_PNS0_5ClassEE",
                                                    *module);
    langClassGetConst = llvm::Function::Create(voidp_2voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang5Class8getConstEPNS0_6SymbolE",
                                               *module);
    langClassSetConst = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang5Class8setConstEPNS0_6SymbolEPNS0_6ObjectE",
                                               *module);
    langSymbolGet = llvm::Function::Create(voidp_1voidp, llvm::Function::ExternalLinkage,
                                           "_ZN9UltraRuby4Lang6Symbol3getEPKc", *module);
    langStringGet = llvm::Function::Create(voidp_1voidp, llvm::Function::ExternalLinkage,
                                           "_ZN9UltraRuby4Lang6String3getEPKc", *module);

    cxaBeginCatch = llvm::Function::Create(voidp_1voidp, llvm::Function::ExternalLinkage,
                                           "__cxa_begin_catch",
                                           *module);
    cxaEndCatch = llvm::Function::Create(void_, llvm::Function::ExternalLinkage,
                                         "__cxa_end_catch",
                                         *module);
    cxaGetExceptionPtr = llvm::Function::Create(voidp_1voidp, llvm::Function::ExternalLinkage,
                                                "__cxa_get_exception_ptr",
                                                *module);
    gxxPersonalityV0 = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage,
                                                llvm::ConstantPointerNull::get(voidpTy), "__gxx_personality_v0");
    exceptionTypeInfo = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                                 "_ZTIN9UltraRuby4Lang9ExceptionE");

    nilConst = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                        "_ZN9UltraRuby4Lang16PrimaryConstants8NilConstE");
    trueConst = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                         "_ZN9UltraRuby4Lang16PrimaryConstants9TrueConstE");
    falseConst = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                          "_ZN9UltraRuby4Lang16PrimaryConstants10FalseConstE");
    rootClass = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                         "_ZN9UltraRuby4Lang12BasicClasses9RootClassE");
}

void CodeGenerator::debugPrintModuleIR() {
    module->dump();
}

bool CodeGenerator::codegenArgsProcessingPreamble(AST::FunctionDef *functionDef, llvm::Function *func) {
    auto argsBegin = func->arg_begin();
    auto argsEnd = func->arg_end();

    auto selfArg = argsBegin;
    auto *selfAlloca = builder->CreateAlloca(voidpTy, nullptr, "self_alloca");
    builder->CreateStore(selfArg, selfAlloca);
    scope->addLocalVariable("self", selfAlloca);
    selfArg->setName("self");

    argsBegin++;
    if (functionDef->hasBlockArg()) {
        argsEnd--;
        auto *blockArg = argsEnd;
        blockArg->setName("blockArg");
        auto *blockAlloca = builder->CreateAlloca(voidpTy, nullptr,
                                                  functionDef->getBlockArg()->getName() + "_alloca");
        builder->CreateStore(blockArg, blockAlloca);
        scope->addLocalVariable(functionDef->getBlockArg()->getName(), blockAlloca);
    }

    llvm::Argument *mapArg = nullptr;
    if (functionDef->hasMapTypeArgs()) {
        argsEnd--;
        mapArg = argsEnd;
        mapArg->setName("mapArg");
    }

    for (const auto &arg: functionDef->getArgs()) {
        switch (arg->getType()) {
            case AST::FuncDefArg::AST_ARG_TYPE_NORMAL: {
                auto *funcArgValue = argsBegin;
                auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_alloca");
                funcArgValue->setName(arg->getName() + "_arg");
                if (arg->getDefaultValue() == nullptr) {
                    builder->CreateStore(funcArgValue, argAlloca);
                } else {
                    auto *cond = builder->CreateICmpNE(funcArgValue, codegenPointer(nullptr),
                                                       "arg_" + arg->getName() + "_isPassed");
                    auto *curBlock = builder->GetInsertBlock();
                    auto *falseBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_useDefault",
                                                                 func);
                    auto *merge = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_merge", func);
                    builder->CreateCondBr(cond, merge, falseBranch);

                    builder->SetInsertPoint(falseBranch);
                    auto *defaultVal = codegenStatement(arg->getDefaultValue());
                    falseBranch = builder->GetInsertBlock();
                    builder->CreateBr(merge);

                    builder->SetInsertPoint(merge);
                    auto *argValPhi = builder->CreatePHI(voidpTy, 2, "arg_" + arg->getName() + "_phi");
                    argValPhi->addIncoming(funcArgValue, curBlock);
                    argValPhi->addIncoming(defaultVal, falseBranch);
                    builder->CreateStore(argValPhi, argAlloca);
                }
                scope->addLocalVariable(arg->getName(), argAlloca);
                argsBegin++;
                break;
            }
            case AST::FuncDefArg::AST_ARG_TYPE_VARIADIC: {
                auto *funcArgValue = argsBegin;
                auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_alloca");
                funcArgValue->setName(arg->getName() + "VariadicArg");
                builder->CreateStore(funcArgValue, argAlloca);
                scope->addLocalVariable(arg->getName(), argAlloca);
                argsBegin++;
                break;
            }
            case AST::FuncDefArg::AST_ARG_TYPE_BLOCK: {
                logError("unpredicted second block arg");
                return false;
            }
            case AST::FuncDefArg::AST_ARG_TYPE_MAP: {
                auto *val = codegenLangCall(langObjectCall[1], std::vector<llvm::Value *>{
                        mapArg, codegenSymbol("has_key?"), codegenSymbol(arg->getName())
                });
                auto *hasKey = codegenCastToBoolInt1(val);
                auto *trueBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_provided", func);
                auto *falseBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_useDefault", func);
                auto *merge = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_merge", func);
                builder->CreateCondBr(hasKey, trueBranch, falseBranch);
                builder->SetInsertPoint(trueBranch);
                auto *externalVal = codegenLangCall(langObjectCall[1], std::vector<llvm::Value *>{
                        mapArg, codegenSymbol("[]"), codegenSymbol(arg->getName())
                });
                trueBranch = builder->GetInsertBlock();
                builder->CreateBr(merge);
                builder->SetInsertPoint(falseBranch);
                auto *defaultVal = codegenStatement(arg->getDefaultValue());
                falseBranch = builder->GetInsertBlock();
                builder->CreateBr(merge);
                builder->SetInsertPoint(merge);
                auto *phi = builder->CreatePHI(voidpTy, 2, "arg_" + arg->getName() + "_phi");
                phi->addIncoming(externalVal, trueBranch);
                phi->addIncoming(defaultVal, falseBranch);
                auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_alloca");;
                builder->CreateStore(phi, argAlloca);
                scope->addLocalVariable(arg->getName(), argAlloca);
                break;
            }
        }
    }
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
    codegenInitializer();
    manager.run(*module);
}

llvm::Value *CodeGenerator::codegenBoolConst(bool val) {
    if (val) {
        return builder->CreateLoad(voidpTy, trueConst);
    } else {
        return builder->CreateLoad(voidpTy, falseConst);
    }
}

llvm::Value *CodeGenerator::codegenBlock(AST::Block *block) {
    llvm::Value *retVal = nullptr;
    for (const auto &stmt: block->getList()) {
        retVal = codegenStatement(stmt);
        if (retVal == nullptr) {
            return nullptr;
        }
    }
    if (retVal == nullptr) {
        retVal = builder->CreateLoad(voidpTy, nilConst);
    }
    return retVal;
}

llvm::Value *CodeGenerator::codegenLocalVariable(AST::LocalVariable *variable) {
    // todo reconsider
    if (variable->getName() == "nil") {
        return builder->CreateLoad(voidpTy, nilConst);
    }
    auto *varPtr = scope->getLocalVariable(variable->getName());
    if (varPtr == nullptr) {
        AST::LangVariable self(AST::LangVariable::LVT_SELF);
        AST::CallArgs callArgs({}, {}, nullptr, false, true);
        AST::Call c(variable->getName(), &callArgs, &self);
        return codegenCall(&c);
    }
    return builder->CreateLoad(voidpTy, varPtr, variable->getName());
}

llvm::Value *CodeGenerator::codegenUnaryOperation(AST::UnaryOperation *unaryOperation) {
    std::string op;
    //todo fix function names
    switch (unaryOperation->getOp()) {
        case AST::UN_OP_NOT:
            op = "!@";
            break;
        case AST::UN_OP_POSITIVE:
            op = "+@";
            break;
        case AST::UN_OP_NEGATIVE:
            op = "-@";
            break;
        case AST::UN_OP_BIN_NEGATION:
            op = "~@";
            break;
        case AST::UN_OP_LIST_UNWRAP:
            op = "*@";
            break;
        default:
            logError("not an unary operation");
            return nullptr;
    }
    AST::CallArgs args(std::vector<AST::Statement *>(), std::map<std::string, AST::Statement *>(), nullptr, false,
                       true);
    AST::Call call(op, &args, unaryOperation->getExpr());
    return codegenCall(&call);
}

llvm::Value *CodeGenerator::codegenIf(AST::If *ifAst) {
    auto *func = builder->GetInsertBlock()->getParent();
    auto *cond = codegenStatement(ifAst->getCondition());
    auto *boolVal = codegenCastToBoolInt1(cond);
    auto *merge = llvm::BasicBlock::Create(*context, "merge", func);
    llvm::BasicBlock *trueBranch = llvm::BasicBlock::Create(*context, "trueBranch", func);
    llvm::BasicBlock *falseBranch = llvm::BasicBlock::Create(*context, "falseBranch", func);
    builder->CreateCondBr(boolVal, trueBranch, falseBranch);
    llvm::Value *trueBranchRetVal;
    llvm::Value *falseBranchRetVal;

    builder->SetInsertPoint(trueBranch);
    if (ifAst->getTrueBranch() != nullptr) {
        trueBranchRetVal = codegenStatement(ifAst->getTrueBranch());
    } else {
        trueBranchRetVal = builder->CreateLoad(voidpTy, nilConst);
    }
    trueBranch = builder->GetInsertBlock();
    builder->CreateBr(merge);

    builder->SetInsertPoint(falseBranch);
    if (ifAst->getFalseBranch() != nullptr) {
        falseBranchRetVal = codegenStatement(ifAst->getFalseBranch());
    } else {
        falseBranchRetVal = builder->CreateLoad(voidpTy, nilConst);
    }
    falseBranch = builder->GetInsertBlock();
    builder->CreateBr(merge);

    builder->SetInsertPoint(merge);
    auto *phi = builder->CreatePHI(voidpTy, 2, "ifRetVal");
    phi->addIncoming(trueBranchRetVal, trueBranch);
    phi->addIncoming(falseBranchRetVal, falseBranch);
    return phi;
}

llvm::Value *CodeGenerator::codegenCastToBoolInt1(llvm::Value *ptr) {
    // ptr != nil & ptr != false -> true
    auto *nilCheck = builder->CreateICmpNE(ptr, builder->CreateLoad(voidpTy, nilConst), "nilCheck");
    auto *boolType = builder->CreateICmpNE(ptr, builder->CreateLoad(voidpTy, falseConst), "falseCheck");
    auto *orVal = builder->CreateAnd(nilCheck, boolType);
    return orVal;
}

llvm::Value *CodeGenerator::codegenClassDef(AST::ClassDef *classDef) {
    auto *classPos = classDef->getClassPos();
    std::string name;
    llvm::Value *outerModule;
    switch (classPos->type) {
        case AST::STMT_CONSTANT_REF: {
            auto *constRef = reinterpret_cast<AST::ConstantRef *>(classPos);
            name = constRef->getName();
            if (constRef->getHolder() == nullptr) {
                outerModule = codegenSelf();
            } else {
                outerModule = codegenStatement(constRef->getHolder());
            }
            break;
        }
        case AST::STMT_LOCAL_VARIABLE: {
            name = reinterpret_cast<AST::LocalVariable *>(classPos)->getName();
            outerModule = codegenSelf();
            break;
        }
        default: {
            logError("expected class name");
            return nullptr;
        }
    }

    AST::FunctionDef functionDef("classDef:" + name + std::to_string(suffix++),
                                 std::vector<AST::FuncDefArg *>(), nullptr, classDef->getDefinition());
    scope->enterNewScope();
    auto *func = codegenFunctionInternal(&functionDef);
    scope->leaveScope();

    auto *superclass = classDef->getSuperclass();
    return codegenLangCall(langObjectDefineClass,
                           {outerModule, codegenSymbol(name), codegenStatement(superclass), func});
}

llvm::Value *CodeGenerator::codegenLangCall(llvm::Function *langFunction, llvm::ArrayRef<llvm::Value *> args) {
    auto *rescueBlock = scope->getHandlerBlock();
    if (rescueBlock != nullptr) {
        auto *normDest = llvm::BasicBlock::Create(*context, "", builder->GetInsertBlock()->getParent(), rescueBlock);
        auto *rv = builder->CreateInvoke(langFunction, normDest, rescueBlock, args);
        builder->SetInsertPoint(normDest);
        return rv;
    }
    return builder->CreateCall(langFunction, args);
}

llvm::Value *CodeGenerator::codegenFor(AST::For *forAst) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenModuleDef(AST::ModuleDef *moduleDef) {
    auto *modulePos = moduleDef->getModulePos();
    std::string name;
    llvm::Value *outerModule;
    switch (modulePos->type) {
        case AST::STMT_CONSTANT_REF: {
            auto *constRef = reinterpret_cast<AST::ConstantRef *>(modulePos);
            name = constRef->getName();
            if (constRef->getHolder() == nullptr) {
                outerModule = codegenSelf();
            } else {
                outerModule = codegenStatement(constRef->getHolder());
            }
            break;
        }
        case AST::STMT_LOCAL_VARIABLE: {
            name = reinterpret_cast<AST::LocalVariable *>(modulePos)->getName();
            outerModule = codegenSelf();
            break;
        }
        default: {
            logError("expected class name");
            return nullptr;
        }
    }

    AST::FunctionDef functionDef("moduleDef:" + name + std::to_string(suffix++),
                                 std::vector<AST::FuncDefArg *>(), nullptr, moduleDef->getDefinition());
    scope->enterNewScope();
    auto *func = codegenFunctionInternal(&functionDef);
    scope->leaveScope();


    return codegenLangCall(langObjectDefineModule, {outerModule, codegenSymbol(name), func});
}

llvm::Value *CodeGenerator::codegenFunctionDef(AST::FunctionDef *functionDef) {
    AST::FunctionDef function("func:" + functionDef->getName(), functionDef->getArgs(),
                              functionDef->getSingleton(), functionDef->getBody());
    scope->enterNewScope();
    auto *func = codegenFunctionInternal(functionDef);
    scope->leaveScope();
    if (func == nullptr) {
        return nullptr;
    }
    llvm::Value *self;
    if (functionDef->getSingleton() != nullptr) {
        self = codegenStatement(functionDef->getSingleton());
    } else {
        self = codegenSelf();
    }

    auto meta = functionDef->createMethodDefMeta();
    auto *cs = llvm::ConstantStruct::get(functionDefMetaType,
                                         std::vector<llvm::Constant *>{
                                                 func,
                                                 builder->getInt8(meta.getRequiredArgsPrefixNum()),
                                                 builder->getInt8(meta.getOptionalArgsNum()),
                                                 builder->getInt8(meta.getRequiredArgsSuffixNum()),
                                                 builder->getInt8(meta.getRequiredArgsTotalNum()),
                                                 builder->getInt8(meta.getDirectArgsNum()),
                                                 builder->getInt8(meta.getFlags()),
                                         });
    auto *var = new llvm::GlobalVariable(*module, functionDefMetaType, false, llvm::GlobalValue::PrivateLinkage, cs,
                                         "functionDefMeta:" + func->getName());
    std::vector<llvm::Value *> args{self, codegenSymbol(functionDef->getName()), var};
    if (functionDef->getSingleton() != nullptr) {
        return codegenLangCall(langObjectDefineSingletonMethod, args);
    } else {
        return codegenLangCall(langObjectDefineInstanceMethod, args);
    }
}

llvm::Value *CodeGenerator::codegenWhile(AST::While *whileAst) {
    auto *func = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *curBlock = builder->GetInsertBlock();
    llvm::BasicBlock *body = llvm::BasicBlock::Create(*context, "body", func);
    llvm::BasicBlock *condCheck = llvm::BasicBlock::Create(*context, "condCheck", func);
    llvm::BasicBlock *merge = llvm::BasicBlock::Create(*context, "merge", func);

    builder->CreateBr(condCheck);

    builder->SetInsertPoint(body);
    auto *retVal = codegenStatement(whileAst->getBody());
    llvm::BasicBlock *bodyEnd = builder->GetInsertBlock();
    builder->CreateBr(condCheck);

    builder->SetInsertPoint(condCheck);
    auto *phi = builder->CreatePHI(voidpTy, 2);
    phi->addIncoming(builder->CreateLoad(voidpTy, nilConst), curBlock);
    phi->addIncoming(retVal, bodyEnd);
    auto *cond = codegenStatement(whileAst->getCondition());
    auto *boolVal = codegenCastToBoolInt1(cond);
    builder->CreateCondBr(boolVal, body, merge);

    builder->SetInsertPoint(merge);
    return phi;
}

llvm::Value *CodeGenerator::codegenClassInstanceDef(AST::ClassInstanceDef *classInstanceDef) {
    AST::FunctionDef functionDef("classInstance:" + std::to_string(suffix++),
                                 std::vector<AST::FuncDefArg *>(),
                                 nullptr, classInstanceDef->getDefinition());
    auto *func = codegenFunctionInternal(&functionDef);
    std::vector<llvm::Value *> callArgs{codegenStatement(classInstanceDef->getInstance()), func};
    return codegenLangCall(langObjectDefineClassInstance, callArgs);
}

llvm::Value *CodeGenerator::codegenBreak(AST::Break *breakAst) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenCall(AST::Call *call) {
    llvm::Value *caller;
    if (call->getObject() != nullptr) {
        caller = codegenStatement(call->getObject());
    } else {
        caller = codegenSelf();
    }
    return codegenCall(caller, call);
}

llvm::Value *CodeGenerator::codegenCall(llvm::Value *caller, AST::Call *call) {
    auto *symVal = codegenSymbol(call->getName());
    auto &args = call->getArgs()->getArgs();
    std::vector<llvm::Value *> callArgs;
    for (const auto &item: args) {
        auto *val = codegenStatement(item);
        if (val == nullptr) {
            return nullptr;
        }
        callArgs.push_back(val);
    }
    return codegenCall(caller, symVal, args.size(), callArgs);
}

llvm::Value *CodeGenerator::codegenCall(llvm::Value *caller, llvm::Value *symVal, int directArgsLen,
                                        llvm::ArrayRef<llvm::Value *> args) {
    if (args.size() <= Lang::Object::MaxDirectArgsLen) {
        std::vector<llvm::Value *> callArgs;
        callArgs.push_back(caller);
        callArgs.push_back(symVal);
        for (const auto &item: args) {
            callArgs.push_back(item);
        }
        return codegenLangCall(langObjectCall[directArgsLen], callArgs);
    } else {
        auto *alloca = builder->CreateAlloca(voidpTy, builder->getInt32(args.size()));
        for (int i = 0; i < args.size(); ++i) {
            builder->CreateStore(args[i], builder->CreateGEP(voidpTy, alloca, {builder->getInt32(i)}));
        }
        return codegenLangCall(langObjectCallV, {caller, symVal, builder->getInt32(directArgsLen), alloca});
    }
}

llvm::Value *CodeGenerator::codegenCase(AST::Case *caseAst) {
    auto *arg = caseAst->getArg();
    auto &vec = caseAst->getCases();
    std::vector<AST::If> ifs;
    std::vector<AST::BinaryOperation> ops;
    ifs.emplace_back(vec.back()->getCond(), vec.back()->getBlock(), nullptr);
    for (int i = vec.size() - 2; i >= 0; ++i) {
        AST::Statement *cond;
        if (arg == nullptr) {
            cond = vec[i]->getCond();
        } else {
            //todo check order
            cond = &ops.emplace_back(AST::OperationType::BIN_OP_CASE_EQUAL, arg, vec[i]->getCond());
        }
        ifs.emplace_back(cond, vec[i]->getBlock(), &ifs.back());
    }
    return codegenIf(&ifs.back());
}

llvm::Value *CodeGenerator::codegenExceptionalBlock(AST::ExceptionalBlock *exceptionalBlock) {
    auto *func = builder->GetInsertBlock()->getParent();
    func->setPersonalityFn(gxxPersonalityV0);

    auto *rescuesBlock = llvm::BasicBlock::Create(*context, "rescue", func);
    auto *ensure = llvm::BasicBlock::Create(*context, "ensure", func);
    auto *rethrow = llvm::BasicBlock::Create(*context, "rethrow", func);

    scope->pushHandlerBlock(rescuesBlock);
    auto *mainRetVal = codegenStatement(exceptionalBlock->getMainBlock());
    auto *mainRetBlock = builder->GetInsertBlock();
    builder->CreateBr(ensure);
    scope->popHandlerBlock();

    builder->SetInsertPoint(ensure);
    auto *exceptionalBlockRetVal = builder->CreatePHI(voidpTy, 2 + exceptionalBlock->getRescues().size());
    auto *needRethrowFlag = builder->CreatePHI(builder->getInt1Ty(), 2 + exceptionalBlock->getRescues().size());
    exceptionalBlockRetVal->addIncoming(mainRetVal, mainRetBlock);
    needRethrowFlag->addIncoming(builder->getInt1(false), mainRetBlock);
    if (exceptionalBlock->getEnsureBlock()) {
        codegenStatement(exceptionalBlock->getEnsureBlock());
    }
    auto *finish = llvm::BasicBlock::Create(*context, "exceptionBlockFinish", func);
    builder->CreateCondBr(needRethrowFlag, rethrow, finish);

    builder->SetInsertPoint(rescuesBlock);
    auto *token = builder->CreateLandingPad(exceptionHandlerType, 1, "exceptionTok");
    token->addClause(exceptionTypeInfo);


    auto *exceptionTypeId = builder->CreateExtractValue(token, {1}, "exceptionTypeID");
    auto *expectedTypeId = builder->CreateIntrinsic(builder->getInt32Ty(), llvm::Intrinsic::eh_typeid_for,
                                                    exceptionTypeInfo);
    auto *matchedTypeId = builder->CreateICmpUGT(exceptionTypeId, exceptionTypeId, "matchedTypeId");

    llvm::BasicBlock *handler = llvm::BasicBlock::Create(*context, "handler", func, finish);
    builder->CreateCondBr(matchedTypeId, handler, rethrow);

    builder->SetInsertPoint(rethrow);
    builder->CreateResume(token);

    builder->SetInsertPoint(handler);
    bool hasFallbackCatch = false;
    if (!exceptionalBlock->getRescues().empty()) {
        auto *abstExceptionPtr = builder->CreateExtractValue(token, {0}, "abstExceptionPtr");
        auto *adjExceptionPtr = codegenLangCall(cxaGetExceptionPtr, {abstExceptionPtr});
        auto *exceptionPtr = builder->CreateGEP(llvm::ArrayType::get(voidpTy, 2), adjExceptionPtr,
                                                {builder->getInt32(0),
                                                 builder->getInt32(sizeof(std::exception) / sizeof(void *))},
                                                "adjExceptionPtr");
        static_assert(sizeof(std::exception) % sizeof(void *) == 0);
        auto *exception = builder->CreateLoad(voidpTy, exceptionPtr, "exceptionObject");
        auto *exceptionClass = codegenGetObjectClass(exception);
        auto &rescues = exceptionalBlock->getRescues();
        for (const auto rescue: rescues) {
            if (rescue->rescueClass != nullptr) {
                auto *rescueClassObject = codegenStatement(rescue->rescueClass);
                auto *rescueClass = codegenGetObjectClass(rescueClassObject);

                auto *isAClass = builder->CreateICmpEQ(rescueClass, codegenPointer(Lang::BasicClasses::ClassClass));
                auto *isAModule = builder->CreateICmpEQ(rescueClass, codegenPointer(Lang::BasicClasses::ModuleClass));
                auto *isValidRescueType = builder->CreateOr(isAClass, isAModule);
                auto *invalidExceptionBlock = llvm::BasicBlock::Create(*context, "invalidExceptionBlock", func, finish);
                auto *validExceptionBlock = llvm::BasicBlock::Create(*context, "validExceptionBlock", func, finish);
                builder->CreateCondBr(isValidRescueType, validExceptionBlock, invalidExceptionBlock);

                builder->SetInsertPoint(invalidExceptionBlock);
                // todo raise "class or module required for rescue clause (TypeError)"
                builder->CreateUnreachable();

                builder->SetInsertPoint(validExceptionBlock);
                // todo check correct order
                auto *compRes = codegenLangCall(langObjectCall[1], {rescueClass, codegenSymbol("<="), exceptionClass});
                auto *clauseMatches = codegenCastToBoolInt1(compRes);
                auto *clause = llvm::BasicBlock::Create(*context, "clause", func, finish);
                handler = llvm::BasicBlock::Create(*context, "handler", func, finish);
                builder->CreateCondBr(clauseMatches, clause, handler);

                builder->SetInsertPoint(clause);
            } else {
                hasFallbackCatch = true;
            }
            if (!rescue->varName.empty()) {
                auto *exceptionVarAloca = builder->CreateAlloca(voidpTy);
                builder->CreateStore(exception, exceptionVarAloca);
                scope->addLocalVariable(rescue->varName, exceptionVarAloca);
            }

            codegenLangCall(cxaBeginCatch, {abstExceptionPtr});
            auto *clauseRetVal = codegenStatement(rescue->handler);
            codegenLangCall(cxaEndCatch, {});

            exceptionalBlockRetVal->addIncoming(clauseRetVal, builder->GetInsertBlock());
            needRethrowFlag->addIncoming(builder->getInt1(false), builder->GetInsertBlock());
            builder->CreateBr(ensure);
            builder->SetInsertPoint(handler);
        }
    } else {

    }
    if (!hasFallbackCatch) {
        exceptionalBlockRetVal->addIncoming(codegenPointer(nullptr), handler);
        needRethrowFlag->addIncoming(builder->getInt1(true), handler);
        builder->CreateBr(ensure);
    }

    builder->SetInsertPoint(finish);
    return exceptionalBlockRetVal;
}

llvm::Value *CodeGenerator::codegenNext(AST::Next *next) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenRedo() {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenRetry() {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenReturn(AST::Return *returnAst) {
    auto &callArgs = returnAst->getCallArgs()->getArgs();
    if (callArgs.empty()) {
        builder->CreateRet(builder->CreateLoad(voidpTy, builder->CreateLoad(voidpTy, nilConst)));
    } else if (callArgs.size() == 1) {
        builder->CreateRet(codegenStatement(callArgs[0]));
    } else {
        AST::Array arr(callArgs);
        builder->CreateRet(codegenArray(&arr));
    }
    //todo handle terminated blocks
    return nullptr;
}

llvm::Value *CodeGenerator::codegenYield(AST::Yield *yield) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenYieldSelf() {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenGetObjectClass(llvm::Value *object) {
    auto *objectClassPtr = builder->CreateGEP(llvm::ArrayType::get(voidpTy, 2), object,
                                              {builder->getInt32(0),
                                               builder->getInt32(Lang::Object::ObjectClassOffset / sizeof(void *))});
    static_assert(Lang::Object::ObjectClassOffset % sizeof(void *) == 0);
    auto *objectClass = builder->CreateLoad(voidpTy, objectClassPtr);
    return objectClass;
}

bool CodeGenerator::codegenInitializer() {
    auto *func = module->getFunction("__init__");
    if (func != nullptr) {
        logError("redefine symbol init function");
        return false;
    }
    auto *ft = llvm::FunctionType::get(voidpTy, {}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__init__", *module);
    auto *entry = llvm::BasicBlock::Create(*context, "", func);
    builder->SetInsertPoint(entry);
    for (const auto &sym: syms) {
        auto *symVal = builder->CreateCall(langSymbolGet, {builder->CreateGlobalString(sym.first)});
        builder->CreateStore(symVal, sym.second);
    }
    for (const auto &str: strs) {
        auto *symVal = builder->CreateCall(langStringGet, {builder->CreateGlobalString(str.first)});
        builder->CreateStore(symVal, str.second);
    }
    builder->CreateRetVoid();
    return true;
}

llvm::Value *CodeGenerator::codegenSelf() {
    AST::LangVariable self(AST::LangVariable::LVT_SELF);
    return codegenLangVariable(&self);
}

llvm::Value *CodeGenerator::codegenInstanceVariable(AST::InstanceVariable *instanceVariable) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenLangVariable(AST::LangVariable *variable) {
    switch (variable->getType()) {
        case AST::LangVariable::LVT_SELF:
            return builder->CreateLoad(voidpTy, scope->getLocalVariable("self"));
        case AST::LangVariable::LVT_NIL:
            return builder->CreateLoad(voidpTy, builder->CreateLoad(voidpTy, nilConst));
    }
}

llvm::Value *CodeGenerator::codegenConstantRef(AST::ConstantRef *constantRef) {
    llvm::Value *outer;
    if (constantRef->getHolder() == nullptr) {
        outer = builder->CreateLoad(voidpTy, rootClass);
    } else {
        outer = codegenStatement(constantRef);
    }
    return codegenLangCall(langClassGetConst, {outer, codegenSymbol(constantRef->getName())});
}
}
}
