#include <iostream>
#include "lexer/Lexer.h"
#include "lexer/input/StringLexerInput.h"
#include "parser/Parser.h"
#include "ir/CodeGenerator.h"
#include "lexer/input/FileLexerInput.h"
#include "ast/AST.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

using namespace UltraRuby;

int main() {
    auto fileLexerInput = std::make_shared<Lexer::FileLexerInput>(
            "/home/user/.local/share/wineprefixes/lona/drive_c/Games/LonaRPG/UltraRevEngine/Main.rb");
    auto lexer = std::make_shared<Lexer::Lexer>(fileLexerInput);
    auto parser = std::make_shared<Parser::Parser>(lexer->getQueue());
    auto block = parser->parseProgram();
    IR::CodeGenerator codeGenerator;
    auto topLevel = std::make_unique<AST::FunctionDef>("top (required)", std::make_unique<AST::FuncDefArgs>(),
                                                       nullptr, std::move(block));
    codeGenerator.codegenProgram(topLevel);
    codeGenerator.debugPrintModuleIR();

    auto targetTriple = llvm::sys::getProcessTriple();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (target == nullptr) {
        std::cout << error << std::endl;
        return -1;
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto rm = std::optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    codeGenerator.setTarget(targetMachine, targetTriple);

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;
    auto filename = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        return 1;
    }

    codeGenerator.runPass(pass);

    dest.flush();
    return 0;
}
