#include "PreambleGenerator.h"
#include "BlockGenerator.h"

namespace UltraRuby {
namespace IR {
PreambleGenerator::PreambleGenerator(FunctionGenerator *functionGenerator) : BlockGenerator(functionGenerator) {
    functionDef = functionGenerator->getFunctionDef();
    func = functionGenerator->getFunc();
}

void PreambleGenerator::emmitIR() {
    auto *preambleBlock = llvm::BasicBlock::Create(*context, "preambleEntry", functionGenerator->getFunc());
    builder = new llvm::IRBuilder<>(preambleBlock);
    argsBegin = functionGenerator->getFunc()->arg_begin();

    codegenSelfExtract();

    if (functionDef->getArgc() == -1) {
        codegenArrayArgsExtract();
    } else {
        codegenDirectArgsExtract();
    }
}

void PreambleGenerator::codegenArrayArgsExtract() {
    argcArg = argsBegin;
    argcArg->setName("argc");
    argvArray = argsBegin + 1;
    argvArray->setName("argv");

    codegenArgcBoundariesCheck();
    codegenExtractSpecialArgs();
    codegenArgumentAllocations();
    codegenExtractOptionalArgs();
    codegenExtractVariadicArray();
    codegenExtractNamedArgs();
}

void PreambleGenerator::codegenExtractNamedArgs() {
    if (!functionDef->getNamedArgs().empty()) {
        for (const auto &arg: functionDef->getNamedArgs()) {
            auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, arg->getName() + "_alloca");;
            functionGenerator->addVariable(arg->getName(), argAlloca);

            auto *val = codegenLangCall(module->getLangObjectCall()[1], std::vector<llvm::Value *>{
                    namedArgsMap, codegenSymbol("has_key?"), codegenSymbol(arg->getName())
            });
            auto *hasKey = codegenCastToBoolInt1(val);
            auto *trueBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_provided", func);
            auto *falseBranch = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_useDefault",
                                                         func);
            auto *merge = llvm::BasicBlock::Create(*context, "arg_" + arg->getName() + "_merge", func);
            builder->CreateCondBr(hasKey, trueBranch, falseBranch);

            builder->SetInsertPoint(trueBranch);
            auto *externalVal = codegenLangCall(module->getLangObjectCall()[1], std::vector<llvm::Value *>{
                    namedArgsMap, codegenSymbol("[]"), codegenSymbol(arg->getName())
            });
            builder->CreateStore(externalVal, argAlloca);
            builder->CreateBr(merge);

            builder->SetInsertPoint(falseBranch);
            if (arg->getDefaultValue()) {
                auto *defaultVal = codegenStatement(arg->getDefaultValue());
                builder->CreateStore(defaultVal, argAlloca);
                builder->CreateBr(merge);
            } else {
                //todo throw missing keyword: :arg_name (ArgumentError)
                builder->CreateUnreachable();
            }
            builder->SetInsertPoint(merge);
        }
        //todo check for unknown keys
    }
}

void PreambleGenerator::codegenExtractVariadicArray() {
    if (!functionDef->getVariadicArg().empty()) {
        auto *variadicAlloca = builder->CreateAlloca(voidpTy, nullptr, functionDef->getVariadicArg() + "_alloca");
        functionGenerator->addVariable(functionDef->getVariadicArg(), variadicAlloca);

        auto *variadicPresent = builder->CreateICmpUGT(argcArg, builder->getInt32(
                functionDef->getRequiredArgsPrefix().size() +
                functionDef->getOptionalArgs().size() +
                functionDef->getRequiredArgsSuffix().size()), "variadicPresent");
        auto *present = llvm::BasicBlock::Create(*context, "variadicPresent", func);
        auto *merge = llvm::BasicBlock::Create(*context, "variadicMerge", func);
        auto *prevBlock = builder->GetInsertBlock();
        builder->CreateCondBr(variadicPresent, present, merge);
        builder->SetInsertPoint(present);
        auto *variadicPresentLength = builder->CreateSub(argcArg, builder->getInt32(
                functionDef->getRequiredArgsPrefix().size() +
                functionDef->getOptionalArgs().size() +
                functionDef->getRequiredArgsSuffix().size()), "variadicPresentSize");
        builder->CreateBr(merge);
        builder->SetInsertPoint(merge);
        auto *phi = builder->CreatePHI(builder->getInt32Ty(), 2, "variadicSize");
        phi->addIncoming(builder->getInt32(0), prevBlock);
        phi->addIncoming(variadicPresentLength, present);
        auto *variadicOffset = builder->CreateGEP(voidpTy, argvArray, {
                builder->getInt32(functionDef->getRequiredArgsPrefix().size() +
                                  functionDef->getOptionalArgs().size())}, "variadicOffset");
        auto *array = codegenLangCall(module->getLangArrayAlloc(), {phi, variadicOffset});
        builder->CreateStore(array, variadicAlloca);
    }
}

void PreambleGenerator::codegenExtractOptionalArgs() {
    if (!functionDef->getOptionalArgs().empty()) {
        for (const auto &defArg: functionDef->getOptionalArgs()) {
            functionGenerator->addVariable(defArg->getName(),
                                           builder->CreateAlloca(voidpTy, nullptr, defArg->getName() + "_alloca"));
        }
        int n = functionDef->getRequiredArgsPrefix().size() + functionDef->getRequiredArgsSuffix().size() + 1;
        int i = functionDef->getRequiredArgsPrefix().size();
        llvm::BasicBlock *prevAbsent = nullptr;
        for (const auto &defArg: functionDef->getOptionalArgs()) {
            auto *present = llvm::BasicBlock::Create(*context, "arg_" + defArg->getName() + "_present", func);
            auto *absent = llvm::BasicBlock::Create(*context, "arg_" + defArg->getName() + "_absent", func);
            auto *optArgPresent = builder->CreateICmpUGE(argcArg, builder->getInt32(n));
            builder->CreateCondBr(optArgPresent, present, absent);

            auto *alloca = functionGenerator->getVariable(defArg->getName());
            assert(alloca && "must be already allocated");

            if (prevAbsent) {
                builder->SetInsertPoint(prevAbsent);
                builder->CreateBr(absent);
            }

            {
                builder->SetInsertPoint(absent);
                assert(defArg->getDefaultValue() && "optional args must have default value");
                auto *v = codegenStatement(defArg->getDefaultValue());
                builder->CreateStore(v, alloca);
                prevAbsent = builder->GetInsertBlock();
            }

            {
                builder->SetInsertPoint(present);
                auto *argPtr = builder->CreateGEP(voidpTy, argvArray, {builder->getInt32(i)},
                                                  defArg->getName() + "_ptr");
                builder->CreateStore(builder->CreateLoad(voidpTy, argPtr), alloca);
            }
            n++;
            i++;
        }

        auto *merge = llvm::BasicBlock::Create(*context, "optionArgsMerge", func);
        builder->CreateBr(merge);
        builder->SetInsertPoint(prevAbsent);
        builder->CreateBr(merge);
        builder->SetInsertPoint(merge);
    }
}

void PreambleGenerator::codegenArgumentAllocations() {
    int i = 0;
    for (const auto &argName: functionDef->getRequiredArgsPrefix()) {
        auto *argPtr = builder->CreateGEP(voidpTy, argvArray, {builder->getInt32(i)}, argName + "_ptr");
        functionGenerator->addVariable(argName, argPtr);
        i++;
    }

    int j = functionDef->getRequiredArgsSuffix().size();
    for (const auto &argName: functionDef->getRequiredArgsSuffix()) {
        auto *pos = builder->CreateSub(argcArg, builder->getInt32(j));
        auto *argPtr = builder->CreateGEP(voidpTy, argvArray, {pos}, argName + "_ptr");
        functionGenerator->addVariable(argName, argPtr);
        j--;
    }
}

void PreambleGenerator::codegenExtractSpecialArgs() {
    if (!functionDef->getBlockArg().empty()) {
        auto *varPtr = builder->CreateThreadLocalAddress(module->getCurrentProc());
        functionGenerator->addVariable(functionDef->getBlockArg(), varPtr);
    }

    namedArgsMap = nullptr;
    if (!functionDef->getNamedArgs().empty()) {
        auto *namedArgsMapPtr = builder->CreateGEP(voidpTy, argvArray, {argcArg}, "namedArgsMapPtr");
        namedArgsMap = builder->CreateLoad(voidpTy, namedArgsMapPtr, "namedArgsMap");
    }
}

void PreambleGenerator::codegenArgcBoundariesCheck() {
    if (!functionDef->getRequiredArgsPrefix().empty() || !functionDef->getRequiredArgsSuffix().empty()) {
        auto *minArgcPassed =
                builder->CreateICmpUGE(argcArg, builder->getInt32(
                                               functionDef->getRequiredArgsPrefix().size() +
                                               functionDef->getRequiredArgsSuffix().size()),
                                       "minArgcPassed");
        auto *failed = llvm::BasicBlock::Create(*context, "minArgsRequirementFailed", func);
        auto *passed = llvm::BasicBlock::Create(*context, "minArgsRequirementSatisfied", func);
        builder->CreateCondBr(minArgcPassed, passed, failed);
        builder->SetInsertPoint(failed);
        //todo throw exception wrong number of arguments (given argc, expected n) (ArgumentError)
        builder->CreateUnreachable();
        builder->SetInsertPoint(passed);
    }

    if (functionDef->getVariadicArg().empty()) {
        auto *maxArgcPassed =
                builder->CreateICmpULE(argcArg, builder->getInt32(
                                               functionDef->getRequiredArgsPrefix().size() +
                                               functionDef->getOptionalArgs().size() +
                                               functionDef->getRequiredArgsSuffix().size()),
                                       "maxArgcPassed");
        auto *failed = llvm::BasicBlock::Create(*context, "maxArgsRequirementFailed", func);
        auto *passed = llvm::BasicBlock::Create(*context, "maxArgsRequirementSatisfied", func);
        builder->CreateCondBr(maxArgcPassed, passed, failed);
        builder->SetInsertPoint(failed);
        //todo throw exception wrong number of arguments (given argc, expected n) (ArgumentError)
        builder->CreateUnreachable();
        builder->SetInsertPoint(passed);
    }
}

void PreambleGenerator::codegenDirectArgsExtract() {
    for (const auto &defArg: functionDef->getRequiredArgsPrefix()) {
        auto *argAlloca = builder->CreateAlloca(voidpTy, nullptr, defArg + "_alloca");
        builder->CreateStore(argsBegin, argAlloca);
        argsBegin->setName(defArg);
        functionGenerator->addVariable(defArg, argAlloca);
        argsBegin++;
    }
}

void PreambleGenerator::codegenSelfExtract() {
    auto selfArg = argsBegin;
    auto *selfAlloca = builder->CreateAlloca(voidpTy, nullptr, "self_alloca");
    builder->CreateStore(selfArg, selfAlloca);
    functionGenerator->addVariable("self", selfAlloca);
    selfArg->setName("self");
    argsBegin++;
}
} // UltraRuby
} // IR