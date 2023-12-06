#include "BlockGenerator.h"
#include "../ast/AST.h"
#include "CodegenException.h"
#include "../lang/BasicClasses.h"

namespace UltraRuby {
namespace IR {
constexpr bool voidpIsUint64 = sizeof(void *) == sizeof(uint64_t);

BlockGenerator::BlockGenerator(FunctionGenerator *functionGenerator) {
    this->functionGenerator = functionGenerator;
    returnValue = nullptr;
    builder = nullptr;
    module = functionGenerator->getModule();
    voidpTy = module->getVoidpTy();
    context = module->getContext();
    voidpTy = module->getVoidpTy();
    block = nullptr;
}

BlockGenerator::BlockGenerator(FunctionGenerator *functionGenerator, AST::Block *block) {
    this->functionGenerator = functionGenerator;
    returnValue = nullptr;
    this->block = block;
    module = functionGenerator->getModule();
    voidpTy = module->getVoidpTy();
    context = module->getContext();
    voidpTy = module->getVoidpTy();
    builder = new llvm::IRBuilder<>(*context);
}

void BlockGenerator::emmitIR() {
    assert(block != nullptr &&
           "call only for BlockGenerator(FunctionGenerator *functionGenerator, AST::Block *block) constructed objects");
    entryBlock = llvm::BasicBlock::Create(*context, "", functionGenerator->getFunc());
    builder->SetInsertPoint(entryBlock);
    returnValue = codegenBlock(block);
}

llvm::Value *BlockGenerator::codegenStatement(AST::Statement *statement) {
    assert(statement != nullptr);
    emmitLocation(statement);
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
            throw CodegenException("Unknown statement", statement->row, statement->col);
        case AST::STMT_LANG_VARIABLE:
            return codegenLangVariable(reinterpret_cast<AST::LangVariable *>(statement));
        case AST::STMT_CONSTANT_REF:
            return codegenConstantRef(reinterpret_cast<AST::ConstantRef *>(statement));
    }
}

llvm::LoadInst *BlockGenerator::codegenSymbol(AST::Symbol *symbol) {
    return codegenSymbol(symbol->getSym());
}

llvm::LoadInst *BlockGenerator::codegenSymbol(const std::string &symbol) {
    auto name = "sym:" + symbol;
    if (auto *l = module->getLLVMModule()->getGlobalVariable(name, true)) {
        return builder->CreateLoad(voidpTy, l);
    }
    auto *variable = new llvm::GlobalVariable(*module->getLLVMModule(), voidpTy, false,
                                              llvm::GlobalValue::PrivateLinkage,
                                              llvm::ConstantPointerNull::get(voidpTy), name);
    module->getSyms().emplace(symbol, variable);
    return builder->CreateLoad(voidpTy, variable);
}

llvm::Value *BlockGenerator::codegenIntegerConst(AST::IntegerConst *intConst) {
    //todo handle cases if val is bigint or is in hex/octal/bin notation
    if (voidpIsUint64) {
        int v = std::stoi(intConst->getVal());
        auto *constVal = builder->getInt64(v);
        return builder->CreateIntToPtr(constVal, voidpTy);
    } else {
        assert(false);
    }
}

llvm::Value *BlockGenerator::codegenFloatConst(AST::FloatConst *floatConst) {
    //todo check machine float size
    if (voidpIsUint64) {
        float v = std::stof(floatConst->getVal());
        return builder->CreateIntToPtr(llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), v), voidpTy);
    } else {
        assert(false);
    }
}

llvm::LoadInst *BlockGenerator::codegenString(AST::String *str) {
    auto name = "str:" + str->getStr();
    if (auto *l = module->getLLVMModule()->getGlobalVariable(name, true)) {
        return builder->CreateLoad(voidpTy, l);
    }
    auto *variable = new llvm::GlobalVariable(*module->getLLVMModule(), voidpTy, false,
                                              llvm::GlobalValue::PrivateLinkage,
                                              llvm::ConstantPointerNull::get(voidpTy), name);
    module->getStrs().emplace(str->getStr(), variable);
    return builder->CreateLoad(voidpTy, variable);
}

llvm::Constant *BlockGenerator::codegenPointer(void *data) {
    if (voidpIsUint64) {
        return llvm::Constant::getIntegerValue(voidpTy, llvm::APInt(64, reinterpret_cast<uint64_t>(data)));
    } else {
        //todo handle 32-bit cases
        assert(false);
    }
}

llvm::Value *BlockGenerator::codegenArray(AST::Array *array) {
    auto elems = array->getElements();
    if (elems.empty()) {
        return codegenLangCall(module->getLangArrayAlloc(), {builder->getInt32(0), codegenPointer(nullptr)});
    }
    auto *arrAlloca = builder->CreateAlloca(voidpTy, builder->getInt32(elems.size()), "arrayTmp");
    for (int i = 0; i < elems.size(); ++i) {
        auto *el = codegenStatement(elems[i]);
        builder->CreateStore(el, builder->CreateGEP(voidpTy, arrAlloca, {builder->getInt32(i)}));
    }
    return codegenLangCall(module->getLangArrayAlloc(), {builder->getInt32(elems.size()), arrAlloca});
}

llvm::Value *BlockGenerator::codegenHash(AST::Hash *hash) {
    auto pairs = hash->getHash();
    if (pairs.empty()) {
        return codegenLangCall(module->getLangHashAlloc(), {builder->getInt32(0), codegenPointer(nullptr)});
    }
    auto *arrAlloca = builder->CreateAlloca(voidpTy, builder->getInt32(pairs.size() * 2), "hashTmp");
    for (int i = 0; i < pairs.size(); ++i) {
        auto *k = codegenStatement(pairs[i].first);
        auto *v = codegenStatement(pairs[i].second);
        builder->CreateStore(k, builder->CreateGEP(voidpTy, arrAlloca, {builder->getInt32(i * 2)}));
        builder->CreateStore(v, builder->CreateGEP(voidpTy, arrAlloca, {builder->getInt32(i * 2 + 1)}));
    }
    return codegenLangCall(module->getLangHashAlloc(), {builder->getInt32(pairs.size()), arrAlloca});
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
        {AST::BIN_OP_ADD,                "+"},
        {AST::BIN_OP_ADD,                "-"},
        {AST::BIN_OP_MULTIPLY,           "*"},
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

llvm::Value *BlockGenerator::codegenBinaryOperation(AST::BinaryOperation *binOp) {
    if (!ops.contains(binOp->getOperator())) {
        throw CodegenException("unexpected binary operator", binOp->row, binOp->col);
    }
    std::string opLabel = ops.at(binOp->getOperator());
    if (AST::BIN_OP_REGION_START <= binOp->getOperator() && binOp->getOperator() < AST::BIN_OP_ASSIGNMENT_START) {
        AST::CallArgs args({binOp->getRight()}, {}, nullptr, false, true);
        AST::Call call(opLabel, &args, binOp->getLeft(), binOp->row, binOp->col);
        return codegenLangCall(module->getLangObjectCall()[1],
                               {codegenStatement(binOp->getLeft()), codegenSymbol(opLabel),
                                codegenStatement(binOp->getRight())});
    } else {
        assert(AST::BIN_OP_ASSIGNMENT_START <= binOp->getOperator() && binOp->getOperator() < AST::BIN_OP_QUESTION &&
               "unknown binary operation");
        auto *left = binOp->getLeft();
        switch (left->type) {
            case AST::STMT_LOCAL_VARIABLE: {
                auto *lv = reinterpret_cast<AST::LocalVariable *>(left);
                auto *alloca = functionGenerator->getVariable(lv->getName());
                if (alloca == nullptr) {
                    alloca = builder->CreateAlloca(voidpTy, nullptr, lv->getName() + "_alloca");
                    emmitLocalVariableDebugInfo(alloca, lv->getName(), left->row, left->col);
                    functionGenerator->addVariable(lv->getName(), alloca);
                }
                auto *rval = codegenStatement(binOp->getRight());
                builder->CreateStore(rval, alloca);
                return rval;
            }
            case AST::STMT_CALL: {
                auto *call = reinterpret_cast<AST::Call *>(left);
                auto *callArgs = call->getArgs();
                if ((callArgs->hasParenthesis() && !call->getName().empty()) || callArgs->getBlock() ||
                    (!callArgs->hasBrackets() && (!callArgs->getArgs().empty()))) {
                    throw CodegenException("unexpected operator-assignment", call->row, call->col);
                }
                if (!callArgs->getNamedArgs().empty()) {
                    //todo
                    throw CodegenException("unimplemented", call->row, call->col);
                }
                auto *lhsContainer = call->getObject();
                auto *lhsContainerVal = lhsContainer != nullptr ? codegenStatement(lhsContainer) : codegenSelf();

                llvm::Value *rhsValue;
                std::string setValMethodName;

                if (callArgs->hasBrackets()) {
                    std::vector<llvm::Value *> bracketArgs;
                    for (const auto &arg: callArgs->getArgs()) {
                        auto *v = codegenStatement(arg);
                        bracketArgs.push_back(v);
                    }
                    auto *lhsValBefore = codegenCall(lhsContainerVal, codegenSymbol("[]"),
                                                     bracketArgs.size(), bracketArgs);
                    auto *rhs = codegenLangCall(module->getLangObjectCall()[1], {lhsValBefore, codegenSymbol(opLabel),
                                                                                 codegenStatement(binOp->getRight())});
                    bracketArgs.push_back(rhs);
                    return codegenCall(lhsContainerVal, codegenSymbol("[]="), bracketArgs.size(), bracketArgs);
                } else {
                    AST::CallArgs setCallArgs({binOp->getRight()}, {}, nullptr, false, true);
                    AST::Call setCall(call->getName() + "=", &setCallArgs, nullptr, binOp->row, binOp->col);
                    return codegenCall(lhsContainerVal, &setCall);
                }
            }
            default: {
                throw CodegenException("unexpected left hand side", left->row, left->col);
            }
        }
    }
}

llvm::Value *BlockGenerator::codegenBoolConst(bool val) {
    if (val) {
        return builder->CreateLoad(voidpTy, module->getTrueConst());
    } else {
        return builder->CreateLoad(voidpTy, module->getFalseConst());
    }
}

llvm::Value *BlockGenerator::codegenBlock(AST::Block *block) {
    llvm::Value *retVal = nullptr;
    for (const auto &stmt: block->getList()) {
        retVal = codegenStatement(stmt);
    }
    if (retVal == nullptr) {
        retVal = builder->CreateLoad(voidpTy, module->getNilConst());
    }
    return retVal;
}

llvm::Value *BlockGenerator::codegenLocalVariable(AST::LocalVariable *variable) {
    auto *varPtr = functionGenerator->getVariable(variable->getName());
    if (varPtr == nullptr) {
        AST::LangVariable self(AST::LangVariable::LVT_SELF, variable->row, variable->col);
        AST::CallArgs callArgs({}, {}, nullptr, false, true);
        AST::Call c(variable->getName(), &callArgs, &self, variable->row, variable->col);
        return codegenCall(&c);
    }
    return builder->CreateLoad(voidpTy, varPtr, variable->getName());
}

llvm::Value *BlockGenerator::codegenUnaryOperation(AST::UnaryOperation *unaryOperation) {
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
            throw CodegenException("not an unary operation", unaryOperation->row, unaryOperation->col);
    }
    AST::CallArgs args(std::vector<AST::Statement *>(), std::map<std::string, AST::Statement *>(), nullptr, false,
                       true);
    AST::Call call(op, &args, unaryOperation->getExpr(), unaryOperation->row, unaryOperation->col);
    return codegenCall(&call);
}

llvm::Value *BlockGenerator::codegenIf(AST::If *ifAst) {
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
        trueBranchRetVal = builder->CreateLoad(voidpTy, module->getNilConst());
    }
    trueBranch = builder->GetInsertBlock();
    builder->CreateBr(merge);

    builder->SetInsertPoint(falseBranch);
    if (ifAst->getFalseBranch() != nullptr) {
        falseBranchRetVal = codegenStatement(ifAst->getFalseBranch());
    } else {
        falseBranchRetVal = builder->CreateLoad(voidpTy, module->getNilConst());
    }
    falseBranch = builder->GetInsertBlock();
    builder->CreateBr(merge);

    builder->SetInsertPoint(merge);
    auto *phi = builder->CreatePHI(voidpTy, 2, "ifRetVal");
    phi->addIncoming(trueBranchRetVal, trueBranch);
    phi->addIncoming(falseBranchRetVal, falseBranch);
    return phi;
}

llvm::Value *BlockGenerator::codegenCastToBoolInt1(llvm::Value *ptr) {
    // ptr != nil & ptr != false -> true
    auto *nilCheck = builder->CreateICmpNE(ptr, builder->CreateLoad(voidpTy, module->getNilConst()), "nilCheck");
    auto *boolType = builder->CreateICmpNE(ptr, builder->CreateLoad(voidpTy, module->getFalseConst()), "falseCheck");
    auto *orVal = builder->CreateAnd(nilCheck, boolType);
    return orVal;
}

llvm::Value *BlockGenerator::codegenClassDef(AST::ClassDef *classDef) {
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
            throw CodegenException("expected class name", classPos->row, classPos->col);
        }
    }

    AST::FunctionDef functionDef("classDef:" + name + ":" + std::to_string(module->getNextSuffix()),
                                 classDef->getDefinition(), classDef->getDefinition()->row,
                                 classDef->getDefinition()->col);
    FunctionGenerator fg(module, &functionDef);
    fg.emmitIR();
    auto *superclass = classDef->getSuperclass();
    return codegenLangCall(module->getLangObjectDefineClass(),
                           {outerModule, codegenSymbol(name), codegenStatement(superclass), fg.getFunc()});
}

llvm::Value *BlockGenerator::codegenLangCall(llvm::Function *langFunction, llvm::ArrayRef<llvm::Value *> args) {
    auto *rescueBlock = functionGenerator->getHandlerBlock();
    if (rescueBlock != nullptr) {
        auto *normDest = llvm::BasicBlock::Create(*context, "", builder->GetInsertBlock()->getParent(), rescueBlock);
        auto *rv = builder->CreateInvoke(langFunction, normDest, rescueBlock, args);
        builder->SetInsertPoint(normDest);
        return rv;
    }
    return builder->CreateCall(langFunction, args);
}

llvm::Value *BlockGenerator::codegenFor(AST::For *forAst) {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenModuleDef(AST::ModuleDef *moduleDef) {
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
            throw CodegenException("expected class name", modulePos->row, modulePos->col);
        }
    }

    AST::FunctionDef functionDef("moduleDef:" + name + ":" + std::to_string(module->getNextSuffix()),
                                 moduleDef->getDefinition(), moduleDef->getDefinition()->row,
                                 moduleDef->getDefinition()->col);
    FunctionGenerator fg(module, &functionDef);
    fg.emmitIR();
    return codegenLangCall(module->getLangObjectDefineModule(), {outerModule, codegenSymbol(name), fg.getFunc()});
}

llvm::Value *BlockGenerator::codegenFunctionDef(AST::FunctionDef *functionDef) {
    AST::FunctionDef function("func:" + functionDef->getName() + ":" + std::to_string(module->getNextSuffix()),
                              functionDef->getBody(), functionDef->row, functionDef->col,
                              functionDef->getRequiredArgsPrefix(), functionDef->getOptionalArgs(),
                              functionDef->getVariadicArg(), functionDef->getRequiredArgsSuffix(),
                              functionDef->getNamedArgs(), functionDef->getBlockArg(), functionDef->getSingleton());
    FunctionGenerator fg(module, &function);
    fg.emmitIR();

    llvm::Value *self;
    if (functionDef->getSingleton() != nullptr) {
        self = codegenStatement(functionDef->getSingleton());
    } else {
        self = codegenSelf();
    }
    std::vector<llvm::Value *> args{self, codegenSymbol(functionDef->getName()), fg.getFunc(),
                                    builder->getInt32(functionDef->getArgc()),
                                    builder->getInt1(!functionDef->getBlockArg().empty()),
                                    builder->getInt1(!functionDef->getNamedArgs().empty())};
    if (functionDef->getSingleton() != nullptr) {
        return codegenLangCall(module->getLangObjectDefineSingletonMethod(), args);
    } else {
        return codegenLangCall(module->getLangObjectDefineInstanceMethod(), args);
    }
}

llvm::Value *BlockGenerator::codegenWhile(AST::While *whileAst) {
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
    phi->addIncoming(builder->CreateLoad(voidpTy, module->getNilConst()), curBlock);
    phi->addIncoming(retVal, bodyEnd);
    auto *cond = codegenStatement(whileAst->getCondition());
    auto *boolVal = codegenCastToBoolInt1(cond);
    builder->CreateCondBr(boolVal, body, merge);

    builder->SetInsertPoint(merge);
    return phi;
}

llvm::Value *BlockGenerator::codegenClassInstanceDef(AST::ClassInstanceDef *classInstanceDef) {
    AST::FunctionDef functionDef("classInstance:" + std::to_string(module->getNextSuffix()),
                                 classInstanceDef->getDefinition(), classInstanceDef->getDefinition()->row,
                                 classInstanceDef->getDefinition()->col);
    FunctionGenerator fg(module, &functionDef);
    fg.emmitIR();
    std::vector<llvm::Value *> callArgs{codegenStatement(classInstanceDef->getInstance()), fg.getFunc()};
    return codegenLangCall(module->getLangObjectDefineClassInstance(), callArgs);
}

llvm::Value *BlockGenerator::codegenBreak(AST::Break *breakAst) {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenCall(AST::Call *call) {
    llvm::Value *caller;
    if (call->getObject() != nullptr) {
        caller = codegenStatement(call->getObject());
    } else {
        caller = codegenSelf();
    }
    return codegenCall(caller, call);
}

llvm::Value *BlockGenerator::codegenCall(llvm::Value *caller, AST::Call *call) {
    auto *symVal = codegenSymbol(call->getName());
    auto &args = call->getArgs()->getArgs();
    std::vector<llvm::Value *> callArgs;
    for (const auto &item: args) {
        callArgs.push_back(codegenStatement(item));
    }
    return codegenCall(caller, symVal, args.size(), callArgs);
}

llvm::Value *BlockGenerator::codegenCall(llvm::Value *caller, llvm::Value *symVal, int directArgsLen,
                                         llvm::ArrayRef<llvm::Value *> args) {
    if (args.size() <= Lang::Object::MaxDirectArgsLen) {
        std::vector<llvm::Value *> callArgs;
        callArgs.push_back(caller);
        callArgs.push_back(symVal);
        for (const auto &item: args) {
            callArgs.push_back(item);
        }
        return codegenLangCall(module->getLangObjectCall()[directArgsLen], callArgs);
    } else {
        auto *alloca = builder->CreateAlloca(voidpTy, builder->getInt32(args.size()));
        for (int i = 0; i < args.size(); ++i) {
            builder->CreateStore(args[i], builder->CreateGEP(voidpTy, alloca, {builder->getInt32(i)}));
        }
        return codegenLangCall(module->getLangObjectCallV(),
                               {caller, symVal, builder->getInt32(directArgsLen), alloca});
    }
}

llvm::Value *BlockGenerator::codegenCase(AST::Case *caseAst) {
    auto *arg = caseAst->getArg();
    auto &vec = caseAst->getCases();
    std::vector<AST::If> ifs;
    std::vector<AST::BinaryOperation> ops;
    ifs.emplace_back(vec.back()->getCond(), vec.back()->getBlock(), nullptr, vec.back()->getCond()->row,
                     vec.back()->getCond()->col);
    for (int i = vec.size() - 2; i >= 0; ++i) {
        AST::Statement *cond;
        if (arg == nullptr) {
            cond = vec[i]->getCond();
        } else {
            //todo check order
            cond = &ops.emplace_back(AST::OperationType::BIN_OP_CASE_EQUAL, arg, vec[i]->getCond(),
                                     vec[i]->getCond()->row, vec[i]->getCond()->col);
        }
        ifs.emplace_back(cond, vec[i]->getBlock(), &ifs.back(), cond->row, cond->col);
    }
    return codegenIf(&ifs.back());
}

llvm::Value *BlockGenerator::codegenExceptionalBlock(AST::ExceptionalBlock *exceptionalBlock) {
    auto *func = builder->GetInsertBlock()->getParent();
    func->setPersonalityFn(module->getGxxPersonalityV0());

    auto *rescuesBlock = llvm::BasicBlock::Create(*context, "rescue", func);
    auto *ensure = llvm::BasicBlock::Create(*context, "ensure", func);
    auto *rethrow = llvm::BasicBlock::Create(*context, "rethrow", func);

    functionGenerator->pushHandlerBlock(rescuesBlock);
    auto *mainRetVal = codegenStatement(exceptionalBlock->getMainBlock());
    auto *mainRetBlock = builder->GetInsertBlock();
    builder->CreateBr(ensure);
    functionGenerator->popHandlerBlock();

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
    auto *token = builder->CreateLandingPad(module->getExceptionHandlerType(), 1, "exceptionTok");
    token->addClause(module->getExceptionTypeInfo());


    auto *exceptionTypeId = builder->CreateExtractValue(token, {1}, "exceptionTypeID");
    auto *expectedTypeId = builder->CreateIntrinsic(builder->getInt32Ty(), llvm::Intrinsic::eh_typeid_for,
                                                    module->getExceptionTypeInfo());
    auto *matchedTypeId = builder->CreateICmpUGT(exceptionTypeId, exceptionTypeId, "matchedTypeId");

    llvm::BasicBlock *handler = llvm::BasicBlock::Create(*context, "handler", func, finish);
    builder->CreateCondBr(matchedTypeId, handler, rethrow);

    builder->SetInsertPoint(rethrow);
    builder->CreateResume(token);

    builder->SetInsertPoint(handler);
    bool hasFallbackCatch = false;
    if (!exceptionalBlock->getRescues().empty()) {
        auto *abstExceptionPtr = builder->CreateExtractValue(token, {0}, "abstExceptionPtr");
        auto *adjExceptionPtr = codegenLangCall(module->getCxaGetExceptionPtr(), {abstExceptionPtr});
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
                auto *compRes = codegenLangCall(module->getLangObjectCall()[1],
                                                {rescueClass, codegenSymbol("<="), exceptionClass});
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
                emmitLocalVariableDebugInfo(exceptionVarAloca, rescue->varName, rescue->handler->row,
                                            rescue->handler->col);
                functionGenerator->addVariable(rescue->varName, exceptionVarAloca);
                builder->CreateStore(exception, exceptionVarAloca);
            }

            codegenLangCall(module->getCxaBeginCatch(), {abstExceptionPtr});
            auto *clauseRetVal = codegenStatement(rescue->handler);
            codegenLangCall(module->getCxaEndCatch(), {});

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

llvm::Value *BlockGenerator::codegenNext(AST::Next *next) {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenRedo() {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenRetry() {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenReturn(AST::Return *returnAst) {
    auto &callArgs = returnAst->getCallArgs()->getArgs();
    if (callArgs.empty()) {
        builder->CreateRet(builder->CreateLoad(voidpTy, module->getNilConst()));
    } else if (callArgs.size() == 1) {
        builder->CreateRet(codegenStatement(callArgs[0]));
    } else {
        AST::Array arr(callArgs, callArgs[0]->row, callArgs[0]->col);
        builder->CreateRet(codegenArray(&arr));
    }
    //todo handle terminated blocks
    return nullptr;
}

llvm::Value *BlockGenerator::codegenYield(AST::Yield *yield) {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenYieldSelf() {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenGetObjectClass(llvm::Value *object) {
    auto *objectClassPtr = builder->CreateGEP(llvm::ArrayType::get(voidpTy, 2), object,
                                              {builder->getInt32(0),
                                               builder->getInt32(Lang::Object::ObjectClassOffset / sizeof(void *))});
    static_assert(Lang::Object::ObjectClassOffset % sizeof(void *) == 0);
    auto *objectClass = builder->CreateLoad(voidpTy, objectClassPtr);
    return objectClass;
}

llvm::Value *BlockGenerator::codegenSelf() {
    AST::LangVariable self(AST::LangVariable::LVT_SELF, -1, -1);
    return codegenLangVariable(&self);
}

llvm::Value *BlockGenerator::codegenInstanceVariable(AST::InstanceVariable *instanceVariable) {
    return nullptr;
}

llvm::Value *BlockGenerator::codegenLangVariable(AST::LangVariable *variable) {
    switch (variable->getType()) {
        case AST::LangVariable::LVT_SELF:
            return builder->CreateLoad(voidpTy, functionGenerator->getVariable("self"));
        case AST::LangVariable::LVT_NIL:
            return builder->CreateLoad(voidpTy, builder->CreateLoad(voidpTy, module->getNilConst()));
    }
}

llvm::Value *BlockGenerator::codegenConstantRef(AST::ConstantRef *constantRef) {
    llvm::Value *outer;
    if (constantRef->getHolder() == nullptr) {
        outer = builder->CreateLoad(voidpTy, module->getRootModule());
    } else {
        outer = codegenStatement(constantRef);
    }
    return codegenLangCall(module->getLangClassGetConst(), {outer, codegenSymbol(constantRef->getName())});
}

void BlockGenerator::emmitLocation(AST::Statement *stmt) {
    if (stmt == nullptr) {
        builder->SetCurrentDebugLocation(llvm::DebugLoc());
    }
    builder->SetCurrentDebugLocation(llvm::DILocation::get(*module->getContext(), stmt->row, stmt->col,
                                                           functionGenerator->getSubprogram()));
}

void BlockGenerator::emmitLocalVariableDebugInfo(llvm::Value *alloca, const std::string &name, int row, int col) {
    auto *paramVarDTy = module->getDiBuilder()->createAutoVariable(
            functionGenerator->getSubprogram(), name, module->getDiCurrentFile(), row,
            module->getObjectPtrDTy(), true);
    module->getDiBuilder()->insertDeclare(
            alloca, paramVarDTy, module->getDiBuilder()->createExpression(),
            llvm::DILocation::get(*module->getContext(), row, col, functionGenerator->getSubprogram()),
            builder->GetInsertBlock());
}
} // UltraRuby
} // IR