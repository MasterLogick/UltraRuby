#ifndef ULTRA_RUBY_PREAMBLEGENERATOR_H
#define ULTRA_RUBY_PREAMBLEGENERATOR_H

#include "FunctionGenerator.h"
#include "BlockGenerator.h"

namespace UltraRuby {
namespace IR {

class PreambleGenerator : private BlockGenerator {
public:
    explicit PreambleGenerator(FunctionGenerator *functionGenerator);

    void emmitIR();

    llvm::IRBuilder<> *getBuilder(){
        return BlockGenerator::getBuilder();
    }
private:
    void codegenSelfExtract();

    void codegenDirectArgsExtract();

    void codegenArrayArgsExtract();

    void codegenArgcBoundariesCheck();

    void codegenExtractSpecialArgs();

    void codegenArgumentAllocations();

    void codegenExtractOptionalArgs();

    void codegenExtractVariadicArray();

    void codegenExtractNamedArgs();

    AST::FunctionDef *functionDef;
    llvm::Function *func;
    llvm::Function::arg_iterator argsBegin;
    llvm::Function::arg_iterator argcArg;
    llvm::Function::arg_iterator argvArray;
    llvm::Value *namedArgsMap;
};

} // UltraRuby
} // IR

#endif //ULTRA_RUBY_PREAMBLEGENERATOR_H
