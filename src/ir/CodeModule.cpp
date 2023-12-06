#include "CodeModule.h"
#include "CodegenException.h"

namespace UltraRuby {
namespace IR {
CodeModule::CodeModule() {
    context = new llvm::LLVMContext();
    new llvm::IRBuilder<>(*context);
    module = new llvm::Module("UltraRuby", *context);
    declareExternLangFunctions();
}

void CodeModule::debugPrintModuleIR() {
    module->dump();
}

void CodeModule::setTarget(llvm::TargetMachine *machine, std::string triple) {
    module->setDataLayout(machine->createDataLayout());
    module->setTargetTriple(triple);
}

void CodeModule::runPass(llvm::legacy::PassManager &manager) {
    codegenInitializer();
    manager.run(*module);
}

void CodeModule::codegenInitializer() {
    auto *func = module->getFunction("__init__");
    if (func != nullptr) {
        throw CodegenException("redefine symbol init function", -1, -1);
    }
    auto *ft = llvm::FunctionType::get(voidpTy, {}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__init__", module);
    auto *entry = llvm::BasicBlock::Create(*context, "", func);
    llvm::IRBuilder<> builder(entry);
    for (const auto &sym: syms) {
        auto *symVal = builder.CreateCall(langSymbolGet, {builder.CreateGlobalString(sym.first)});
        builder.CreateStore(symVal, sym.second);
    }
    for (const auto &str: strs) {
        auto *symVal = builder.CreateCall(langStringGet, {builder.CreateGlobalString(str.first)});
        builder.CreateStore(symVal, str.second);
    }
    builder.CreateRetVoid();
}

void CodeModule::declareExternLangFunctions() {
    voidpTy = llvm::PointerType::getUnqual(*context);
    int8Ty = llvm::Type::getInt8Ty(*context);
    auto *int32Ty = llvm::Type::getInt32Ty(*context);
    auto *voidp_intvoidp = llvm::FunctionType::get(voidpTy, {int32Ty, voidpTy}, false);

    auto *void_ = llvm::FunctionType::get(voidpTy, {}, false);
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
    auto *voidp_2voidpintvoidp = llvm::FunctionType::get(voidpTy, {voidpTy, voidpTy, int32Ty, voidpTy}, false);
    auto *voidp_3voidpintvoidp = llvm::FunctionType::get(voidpTy, {voidpTy, voidpTy, voidpTy, int32Ty, voidpTy}, false);
    auto *voidp_4voidpintvoidp = llvm::FunctionType::get(voidpTy,
                                                         {voidpTy, voidpTy, voidpTy, voidpTy, int32Ty, voidpTy}, false);
    auto *int1Ty = llvm::Type::getInt1Ty(*context);
    auto *voidp_3voidp1int2bool = llvm::FunctionType::get(voidpTy, {voidpTy, voidpTy, voidpTy, int32Ty, int1Ty, int1Ty},
                                                          false);

    functionDefMetaType = llvm::StructType::create(*context, "FunctionDefMeta");
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

    langObjectDefineInstanceMethod = llvm::Function::Create(voidp_3voidp1int2bool, llvm::Function::ExternalLinkage,
                                                            "_ZN9UltraRuby4Lang6Object20defineInstanceMethodEPNS0_6SymbolEPvibb",
                                                            *module);
    langObjectDefineSingletonMethod = llvm::Function::Create(voidp_3voidp1int2bool, llvm::Function::ExternalLinkage,
                                                             "_ZN9UltraRuby4Lang6Object21defineSingletonMethodEPNS0_6SymbolEPvibb",
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
                                               "_ZN9UltraRuby4Lang6Module8getConstEPNS0_6SymbolE",
                                               *module);
    langClassSetConst = llvm::Function::Create(voidp_3voidp, llvm::Function::ExternalLinkage,
                                               "_ZN9UltraRuby4Lang6Module8setConstEPNS0_6SymbolEPNS0_6ObjectE",
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
    rootModule = new llvm::GlobalVariable(*module, voidpTy, true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                          "_ZN9UltraRuby4Lang12BasicClasses10RootModuleE");
    currentProc = new llvm::GlobalVariable(*module, voidpTy, false, llvm::GlobalValue::ExternalLinkage, nullptr,
                                           "_ZN9UltraRuby4Lang6Object11currentProcE", nullptr,
                                           llvm::GlobalValue::GeneralDynamicTLSModel);
}

} // UltraRuby
} // IR