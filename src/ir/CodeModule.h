#ifndef ULTRA_RUBY_CODEMODULE_H
#define ULTRA_RUBY_CODEMODULE_H

#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>
#include <map>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include "../lang/Object.h"

namespace UltraRuby {
namespace IR {

class CodeModule {
public:
    CodeModule();

    void debugPrintModuleIR();

    void setFile(std::string name, std::string directory, std::string source);

    void setTarget(llvm::TargetMachine *machine, std::string triple);

    void runPass(llvm::legacy::PassManager &manager);

    llvm::LLVMContext *getContext() {
        return context;
    }

    llvm::Module *getLLVMModule() {
        return module;
    }

    std::map<std::string, llvm::GlobalVariable *> &getSyms() {
        return syms;
    }

    std::map<std::string, llvm::GlobalVariable *> &getStrs() {
        return strs;
    }

    int getNextSuffix() {
        return ++suffix;
    }

    llvm::PointerType *getVoidpTy() const {
        return voidpTy;
    }

    llvm::Type *getInt64Ty() const {
        return int64Ty;
    }

    llvm::Type *getInt8Ty() const {
        return int8Ty;
    }

    llvm::StructType *getFunctionDefMetaType() const {
        return functionDefMetaType;
    }

    llvm::StructType *getExceptionHandlerType() const {
        return exceptionHandlerType;
    }

    llvm::Constant *getNilConst() const {
        return nilConst;
    }

    llvm::Constant *getTrueConst() const {
        return trueConst;
    }

    llvm::Constant *getFalseConst() const {
        return falseConst;
    }

    llvm::Constant *getRootModule() const {
        return rootModule;
    }

    llvm::Function *getLangArrayAlloc() const {
        return langArrayAlloc;
    }

    llvm::Function *getLangHashAlloc() const {
        return langHashAlloc;
    }

    llvm::Function *const *getLangObjectCall() const {
        return langObjectCall;
    }

    llvm::Function *getLangObjectCallV() const {
        return langObjectCallV;
    }

    llvm::Function *getLangObjectCallBv() const {
        return langObjectCallBV;
    }

    llvm::Function *getLangObjectCallNv() const {
        return langObjectCallNV;
    }

    llvm::Function *getLangObjectCallNbv() const {
        return langObjectCallNBV;
    }

    llvm::Function *getLangObjectDefineInstanceMethod() const {
        return langObjectDefineInstanceMethod;
    }

    llvm::Function *getLangObjectDefineSingletonMethod() const {
        return langObjectDefineSingletonMethod;
    }

    llvm::Function *getLangObjectDefineClassInstance() const {
        return langObjectDefineClassInstance;
    }

    llvm::Function *getLangObjectDefineClass() const {
        return langObjectDefineClass;
    }

    llvm::Function *getLangObjectDefineModule() const {
        return langObjectDefineModule;
    }

    llvm::Function *getLangClassGetConst() const {
        return langClassGetConst;
    }

    llvm::Function *getLangClassSetConst() const {
        return langClassSetConst;
    }

    llvm::Function *getLangSymbolGet() const {
        return langSymbolGet;
    }

    llvm::Function *getLangStringGet() const {
        return langStringGet;
    }

    llvm::Function *getCxaBeginCatch() const {
        return cxaBeginCatch;
    }

    llvm::Function *getCxaEndCatch() const {
        return cxaEndCatch;
    }

    llvm::Constant *getGxxPersonalityV0() const {
        return gxxPersonalityV0;
    }

    llvm::Function *getCxaGetExceptionPtr() const {
        return cxaGetExceptionPtr;
    }

    llvm::Constant *getExceptionTypeInfo() const {
        return exceptionTypeInfo;
    }

    llvm::GlobalVariable *getCurrentProc() const {
        return currentProc;
    }

    llvm::DIBuilder *getDiBuilder() const {
        return diBuilder;
    }

    llvm::DICompileUnit *getDiCompileUnit() const {
        return diCompileUnit;
    }

    llvm::DIFile *getDiCurrentFile() const {
        return diCurrentFile;
    }

    llvm::DIType *getObjectPtrDTy() const {
        return objectPtrDTy;
    }

    llvm::DIType *getObjectArrPtrDTy() const {
        return objectArrPtrDTy;
    }

    llvm::DIBasicType *getInt32DTy() const {
        return int32DTy;
    }

    llvm::DISubroutineType *getVariadicSubroutineDTy() const {
        return variadicSubroutineDTy;
    }

private:
    void codegenInitializer();

    void declareExternLangFunctions();

    llvm::LLVMContext *context;
    llvm::Module *module;
    std::map<std::string, llvm::GlobalVariable *> syms;
    std::map<std::string, llvm::GlobalVariable *> strs;
    int suffix;

    llvm::PointerType *voidpTy;
    llvm::Type *int64Ty;
    llvm::Type *int8Ty;
    llvm::StructType *functionDefMetaType;
    llvm::StructType *exceptionHandlerType;

    llvm::Constant *nilConst;
    llvm::Constant *trueConst;
    llvm::Constant *falseConst;
    llvm::Constant *rootModule;

    llvm::Function *langArrayAlloc;
    llvm::Function *langHashAlloc;
    llvm::Function *langObjectCall[Lang::Object::MaxDirectArgsLen + 1];
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

    llvm::GlobalVariable *currentProc;

    llvm::DIBuilder *diBuilder;
    llvm::DICompileUnit *diCompileUnit;
    llvm::DIFile *diCurrentFile;
    llvm::DIType *objectPtrDTy;
    llvm::DIType *objectArrPtrDTy;
    llvm::DIBasicType *int32DTy;
    llvm::DISubroutineType *variadicSubroutineDTy;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_CODEMODULE_H
