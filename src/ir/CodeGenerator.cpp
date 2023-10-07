#include <iostream>
#include "CodeGenerator.h"
#include "../lang/Symbol.h"
#include "../lang/String.h"
#include "../ast/AST.h"
#include "../lang/PrimaryConstants.h"
#include <llvm/IR/Verifier.h>

namespace UltraRuby {
namespace IR {
constexpr bool voidpIsUint64 = sizeof(void *) == sizeof(uint64_t);

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("ultra-ruby", *context);
    declareExternLangFunctions();
}

llvm::Function *CodeGenerator::codegenProgram(AST::FunctionDef *topLevelDecl) {
    scope = new Scope();
    return codegenFunction(topLevelDecl);
}

llvm::Value *CodeGenerator::codegenStatement(AST::Statement *statement) {
    switch (statement->type) {
        case AST::STMT_UNKNOWN:
            return nullptr;
        case AST::STMT_BIN_OP:
            return codegenBinaryOperation(static_cast<AST::BinaryOperation *>(statement));
        case AST::STMT_UN_OP:
            return codegenUnaryOperation(static_cast<AST::UnaryOperation *>(statement));
        case AST::STMT_CALL:
            return codegenCall(static_cast<AST::Call *>(statement));
        case AST::STMT_FLOAT_CONST:
            return codegenFloatConst(static_cast<AST::FloatConst *>(statement));
        case AST::STMT_INT_CONST:
            return codegenIntegerConst(static_cast<AST::IntegerConst *>(statement));
        case AST::STMT_STRING:
            return codegenString(static_cast<AST::String *>(statement));
        case AST::STMT_SYMBOL:
            return codegenSymbol(static_cast<AST::Symbol *>(statement));
        case AST::STMT_ARRAY:
            return codegenArray(static_cast<AST::Array *>(statement));
        case AST::STMT_HASH:
            return codegenHash(static_cast<AST::Hash *>(statement));
        case AST::STMT_VARIABLE:
            return codegenVariable(static_cast<AST::Variable *>(statement));
        case AST::STMT_CLASS_DEF:
            return codegenClassDef(static_cast<AST::ClassDef *>(statement));
        case AST::STMT_MODULE_DEF:
            return codegenModuleDef(static_cast<AST::ModuleDef *>(statement));
        case AST::STMT_FUNC_DEF:
            return codegenFunctionDef(static_cast<AST::FunctionDef *>(statement));
        case AST::STMT_CLASS_INSTANCE_DEF:
            return codegenClassInstanceDef(static_cast<AST::ClassInstanceDef *>(statement));
        case AST::STMT_BLOCK:
            return codegenBlock(static_cast<AST::Block *>(statement));
        case AST::STMT_EXCEPTIONAL_BLOCK:
            return codegenExceptionalBlock(static_cast<AST::ExceptionalBlock *>(statement));
        case AST::STMT_IF:
            return codegenIf(static_cast<AST::If *>(statement));
        case AST::STMT_WHILE:
            return codegenWhile(static_cast<AST::While *>(statement));
        case AST::STMT_CASE:
            return codegenCase(static_cast<AST::Case *>(statement));
        case AST::STMT_FOR:
            return codegenFor(static_cast<AST::For *>(statement));
        case AST::STMT_BOOL_CONST:
            return codegenBoolConst(static_cast<AST::BoolConst *>(statement)->getVal());
        case AST::STMT_RETURN:
            return codegenReturn(static_cast<AST::Return *>(statement));
        case AST::STMT_NEXT:
            return codegenNext(static_cast<AST::Next *>(statement));
        case AST::STMT_BREAK:
            return codegenBreak(static_cast<AST::Break *>(statement));
        case AST::STMT_REDO:
            return codegenBinaryOperation(static_cast<AST::BinaryOperation *>(statement));
        case AST::STMT_RETRY:
            return codegenRetry();
        case AST::STMT_YIELD:
            return codegenYield(static_cast<AST::Yield *>(statement));
        case AST::STMT_YIELD_SELF:
            return codegenYieldSelf();
    }
}

llvm::Function *CodeGenerator::codegenFunctionBody(AST::FunctionDef *functionDef) {
    auto name = scope->deriveFunctionName(functionDef);
    auto *func = module->getFunction(name);
    if (func != nullptr) {
        logError("function redeclaration");
        return nullptr;
    }
    auto args = functionDef->getArgs();
    std::vector<llvm::Type *> argTypes(functionDef->getMaxArgsCount() + 1, voidpTy);
    auto *ft = llvm::FunctionType::get(voidpTy, argTypes, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, *module);
    scope = scope->enterFunctionBody(functionDef);
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
    return func;
}

llvm::Constant *CodeGenerator::codegenSymbol(AST::Symbol *symbol) {
    auto *s = Lang::Symbol::get(symbol->getSym());
    return codegenPointer(s);
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
    //todo check ruby impl float size
    if (voidpIsUint64) {
        float v = std::stof(floatConst->getVal());
        return builder->CreateIntToPtr(llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), v), voidpTy);
    } else {
        assert(false);
    }
}

llvm::Constant *CodeGenerator::codegenString(AST::String *str) {
    auto *s = Lang::String::get(str->getStr());
    return codegenPointer(s);
}

llvm::Constant *CodeGenerator::codegenPointer(void *data) {
    if (voidpIsUint64) {
        return llvm::Constant::getIntegerValue(voidpTy, llvm::APInt(64, reinterpret_cast<uint64_t>(data)));
    } else {
        //todo handle 32-bit cases
        assert(false);
    }
}

llvm::Value *CodeGenerator::codegenArray(AST::Array *array) {
    std::vector<llvm::Value *> values(array->getElements().size() + 1);
    values.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context), llvm::APInt(32, array->getElements().size())));
    for (const auto &item: array->getElements()) {
        values.push_back(codegenStatement(item));
    }
    return builder->CreateCall(langArrayAlloc->getFunctionType(), langArrayAlloc, values, "array");
}

llvm::Value *CodeGenerator::codegenHash(AST::Hash *hash) {
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

llvm::Value *CodeGenerator::codegenBinaryOperation(AST::BinaryOperation *binOp) {
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
        case AST::BIN_OP_CASE_EQUAL:
            opLabel = "===";
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
        AST::CallArgs args(std::vector<AST::Statement *>{binOp->getRight()}, nullptr, false, false);
        AST::Call call(opLabel, &args, binOp->getLeft());
        return codegenCall(&call);
    } else {
        //todo
        return nullptr;
    }
}

void CodeGenerator::declareExternLangFunctions() {
    std::vector<llvm::Type *> argTypes;
    voidpTy = llvm::PointerType::getUnqual(*context);
    int64Ty = llvm::Type::getInt64Ty(*context);
    auto *int32Ty = llvm::Type::getInt32Ty(*context);
    auto *voidp_IntVariadic = llvm::FunctionType::get(
            voidpTy,
            std::vector<llvm::Type *>{int32Ty},
            true);
    auto *voidp_2voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(2, voidpTy), false);
    auto *voidp_3voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(3, voidpTy), false);
    auto *voidp_4voidp = llvm::FunctionType::get(voidpTy,
                                                 std::vector<llvm::Type *>(4, voidpTy), false);
    auto *voidp_2voidp2intvoidp = llvm::FunctionType::get(
            voidpTy,
            std::vector<llvm::Type *>{voidpTy, voidpTy, int32Ty, int32Ty, voidpTy},
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
    langClassDefineClass = llvm::Function::Create(voidp_4voidp, llvm::Function::ExternalLinkage,
                                                  "_ZN9UltraRuby4Lang5Class11defineClassEPNS0_6SymbolEPPS1_S4_PFPNS0_6ObjectES4_E",
                                                  *module);
    langModuleDefineModule = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                                    "_ZN9UltraRuby4Lang6Module12defineModuleEPNS0_6SymbolEPPS1_PFPNS0_6ObjectES4_E",
                                                    *module);

    nilConst = codegenPointer(Lang::PrimaryConstants::nilConst);
    trueConst = codegenPointer(Lang::PrimaryConstants::trueConst);
    falseConst = codegenPointer(Lang::PrimaryConstants::falseConst);
}

void CodeGenerator::debugPrintModuleIR() {
    module->dump();
}

bool CodeGenerator::codegenArgsProcessingPreamble(AST::FunctionDef *functionDef, llvm::Function *func) {
    auto argsBegin = func->args().begin();
    auto argsEnd = func->args().end();

    auto selfArg = argsBegin;
    scope->addVariable("self", selfArg);
    selfArg->setName("self");

    argsBegin++;
    if (functionDef->hasBlockArg()) {
        argsEnd--;
        auto *blockArg = argsEnd;
        blockArg->setName("blockArg");
        auto *blockAlloca = builder->CreateAlloca(voidpTy, nullptr,
                                                  functionDef->getBlockArg()->getName() + "_varAllocaPtr");
        builder->CreateStore(blockArg, blockAlloca);
        scope->addVariable(functionDef->getBlockArg()->getName(), blockAlloca);
    }

    llvm::Argument *mapArg = nullptr;
    if (functionDef->hasMapTypeArgs()) {
        argsEnd--;
        mapArg = argsEnd;
        mapArg->setName("mapArg");
    }
    AST::Symbol hasKeySym("has_key?");
    AST::Symbol getSym("[]");

    for (const auto &arg: functionDef->getArgs()) {
        switch (arg->getType()) {
            case AST::FuncDefArg::AST_ARG_TYPE_NORMAL: {
                auto *funcArgValue = argsBegin;
                auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_varAllocaPtr");
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
                scope->addVariable(arg->getName(), argAlloca);
                argsBegin++;
                break;
            }
            case AST::FuncDefArg::AST_ARG_TYPE_VARIADIC: {
                auto *funcArgValue = argsBegin;
                auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_varAllocaPtr");
                funcArgValue->setName(arg->getName() + "VariadicArg");
                builder->CreateStore(funcArgValue, argAlloca);
                scope->addVariable(arg->getName(), argAlloca);
                argsBegin++;
                break;
            }
            case AST::FuncDefArg::AST_ARG_TYPE_BLOCK: {
                logError("unpredicted second block arg");
                return false;
            }
            case AST::FuncDefArg::AST_ARG_TYPE_MAP: {
                AST::Symbol nameSym(arg->getName());
                auto *val = codegenLangCall(langObjectCall1, std::vector<llvm::Value *>{
                        mapArg, codegenSymbol(&hasKeySym), codegenSymbol(&nameSym)
                });
                auto *hasKey = codegenCastToBoolInt1(val);
                auto *trueBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_provided", func);
                auto *falseBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_useDefault", func);
                auto *merge = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_merge", func);
                builder->CreateCondBr(hasKey, trueBranch, falseBranch);
                builder->SetInsertPoint(trueBranch);
                auto *externalVal = codegenLangCall(langObjectCall1, std::vector<llvm::Value *>{
                        mapArg, codegenSymbol(&getSym), codegenSymbol(&nameSym)
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
                auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_varAllocaPtr");;
                builder->CreateStore(phi, argAlloca);
                scope->addVariable(arg->getName(), argAlloca);
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
    manager.run(*module);
}

llvm::Constant *CodeGenerator::codegenBoolConst(bool val) {
    if (val) {
        return trueConst;
    } else {
        return falseConst;
    }
}

llvm::Value *CodeGenerator::codegenBlock(AST::Block *block) {
    llvm::Value *retVal = nilConst;
    for (const auto &stmt: block->getList()) {
        retVal = codegenStatement(stmt);
    }
    return retVal;
}

llvm::Value *CodeGenerator::codegenVariable(AST::Variable *variable) {
    auto *varPtr = scope->getVariable(variable->getName());
    if (variable->getName() == "self") {
        return varPtr;
    }
    return builder->CreateLoad(voidpTy, varPtr, variable->getName());
}

llvm::Value *CodeGenerator::codegenUnaryOperation(AST::UnaryOperation *unaryOperation) {
    std::string op;
    switch (unaryOperation->getOp()) {
        case AST::UN_OP_NOT:
            op = "!@";
            break;
        case AST::UN_OP_POSITIVE:
            op = "+@";
            break;
        case AST::UN_OP_NEGATION:
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
    AST::CallArgs args(std::vector<AST::Statement *>(), nullptr, false, false);
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
        trueBranchRetVal = nilConst;
    }
    trueBranch = builder->GetInsertBlock();
    builder->CreateBr(merge);

    builder->SetInsertPoint(falseBranch);
    if (ifAst->getFalseBranch() != nullptr) {
        falseBranchRetVal = codegenStatement(ifAst->getFalseBranch());
    } else {
        falseBranchRetVal = nilConst;
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
//    auto *intVal = builder->CreatePtrToInt(ptr, int64Ty, "ptrIntVal");
    auto *nilCheck = builder->CreateICmpNE(ptr, nilConst, "nilCheck");
    auto *boolType = builder->CreateICmpNE(ptr, falseConst, "falseCheck");
    auto *orVal = builder->CreateAnd(nilCheck, boolType);
    return orVal;
}

llvm::Value *CodeGenerator::codegenClassDef(AST::ClassDef *classDef) {
    AST::FunctionDef functionDef(scope->deriveClassDeclFunction(classDef), std::vector<AST::FuncDefArg *>(), nullptr,
                                 classDef->getDefinition());
    auto *func = codegenFunction(&functionDef);
    AST::Symbol className(classDef->getClassIdentifier());
    Lang::Class **classPtrLocation = scope->getOrAllocClass(classDef);
    std::vector<llvm::Value *> callArgs{codegenSymbol(&className), codegenPointer(classPtrLocation),
                                        codegenPointer(scope->getSuperClass(classDef)), func};
    return codegenLangCall(langClassDefineClass, callArgs);
}

llvm::Value *CodeGenerator::codegenLangCall(llvm::Function *langFunction, std::vector<llvm::Value *> args) {
    return builder->CreateCall(langFunction, args);
}

llvm::Value *CodeGenerator::codegenFor(AST::For *forAst) {
//    forAst.;
}

llvm::Value *CodeGenerator::codegenModuleDef(AST::ModuleDef *moduleDef) {
    AST::FunctionDef functionDef(scope->deriveModuleDeclFunction(moduleDef), std::vector<AST::FuncDefArg *>(), nullptr,
                                 moduleDef->getDefinition());
    auto *func = codegenFunction(&functionDef);
    AST::Symbol className(moduleDef->getModuleIdentifier());
    Lang::Class **classPtrLocation = scope->getOrAllocModule(moduleDef);
    std::vector<llvm::Value *> callArgs{codegenSymbol(&className), codegenPointer(classPtrLocation), func};
    return codegenLangCall(langModuleDefineModule, callArgs);
}

llvm::Value *CodeGenerator::codegenFunctionDef(AST::FunctionDef *functionDef) {
    auto *func = codegenFunction(functionDef);
    llvm::Value *self;
    if (functionDef->getSingleton() != nullptr) {
        self = codegenStatement(functionDef->getSingleton());
    } else {
        AST::Variable s("self");
        self = codegenVariable(&s);
    }
    AST::Symbol sym(functionDef->getName());
    std::vector<llvm::Value *> args{self, codegenSymbol(&sym),
                                    builder->getInt32(functionDef->getMinArgsCount()),
                                    builder->getInt32(functionDef->getMaxArgsCount()),
                                    func};
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
    phi->addIncoming(nilConst, curBlock);
    phi->addIncoming(retVal, bodyEnd);
    auto *cond = codegenStatement(whileAst->getCondition());
    auto *boolVal = codegenCastToBoolInt1(cond);
    builder->CreateCondBr(boolVal, body, merge);

    builder->SetInsertPoint(merge);
    return phi;
}

llvm::Value *CodeGenerator::codegenClassInstanceDef(AST::ClassInstanceDef *classInstanceDef) {
    AST::FunctionDef functionDef(scope->deriveClassInstanceMethod(classInstanceDef), std::vector<AST::FuncDefArg *>(),
                                 nullptr, classInstanceDef->getDefinition());
    auto *func = codegenFunction(&functionDef);
    std::vector<llvm::Value *> callArgs{codegenStatement(classInstanceDef->getInstance()), func};
    return codegenLangCall(langObjectDefineSingletonMethod, callArgs);
}

llvm::Value *CodeGenerator::codegenBreak(AST::Break *breakAst) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenCall(AST::Call *call) {
    llvm::Value *caller;
    AST::Symbol sym(call->getName());
    auto *symVal = codegenSymbol(&sym);
    if (call->getObject() != nullptr) {
        caller = codegenStatement(call->getObject());
    } else {
        AST::Variable self("self");
        caller = codegenVariable(&self);
    }
    return codegenLangCall(langObjectCall0, std::vector<llvm::Value *>{caller, symVal});
}

llvm::Value *CodeGenerator::codegenCase(AST::Case *caseAst) {
    auto *arg = caseAst->getArg();
    auto &vec = caseAst->getCases();
    // stores all temporary ast nodes in vectors to prevent memory leaks
    std::vector<AST::If> ifs;
    std::vector<AST::BinaryOperation> ops;
    ifs.emplace_back(vec.back()->getCond(), vec.back()->getBlock(), nullptr);
    for (int i = vec.size() - 2; i >= 0; ++i) {
        AST::Statement *cond;
        if (arg == nullptr) {
            cond = vec[i]->getCond();
        } else {
            cond = &ops.emplace_back(AST::OperationType::BIN_OP_CASE_EQUAL, arg, vec[i]->getCond());
        }
        ifs.emplace_back(cond, vec[i]->getBlock(), &ifs.back());
    }
    return codegenIf(&ifs.back());
}

llvm::Value *CodeGenerator::codegenExceptionalBlock(AST::ExceptionalBlock *exceptionalBlock) {
    return nullptr;
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
        builder->CreateRet(nilConst);
    } else if (callArgs.size() == 1) {
        builder->CreateRet(codegenStatement(callArgs[0]));
    } else {
        AST::Array arr(callArgs);
        builder->CreateRet(codegenArray(&arr));
    }
    scope->markBlockAsTerminated();
    //todo handle terminated blocks
}

llvm::Value *CodeGenerator::codegenYield(AST::Yield *yield) {
    return nullptr;
}

llvm::Value *CodeGenerator::codegenYieldSelf() {
    return nullptr;
}

llvm::Function *CodeGenerator::codegenFunction(AST::FunctionDef *functionDef) {
    auto *block = builder->GetInsertBlock();
    auto iter = builder->GetInsertPoint();
    auto *retVal = codegenFunctionBody(functionDef);
    if (block != nullptr) {
        builder->SetInsertPoint(block, iter);
    }
    return retVal;
}
}
}
