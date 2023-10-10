#include <iostream>
#include "lexer/Lexer.h"
#include "lexer/input/StringLexerInput.h"
#include "parser/Parser.h"
#include "ir/CodeGenerator.h"
#include "lexer/input/FileLexerInput.h"
#include "ast/AST.h"
#include "lang/BasicClasses.h"
#include "lang/PrimaryConstants.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include "lang/Symbol.h"

using namespace UltraRuby;

int a(int x) {
    return x * 2;
}

int b(int x) {
    return x + 2;
}

int c(int x) {
    return x - 2;
}

int main() {
/*    int r[3];
    std::map<void *, int (*)(int)> m{
            {r,     a},
            {r + 1, b},
            {r + 2, c},
    };
    typedef int(*rrr)(int);
    rrr arr[3] = {a, b, c};
    int f = 5;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1'000'000'000; ++i) {
        f = a(f);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1'000'000'000; ++i) {
        f = arr[i % 3](f);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100'000'000; ++i) {
        f = m[r + i % 3](f);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;*/

    Lang::BasicClasses::init();
    Lang::PrimaryConstants::init();

    auto stringLexerInput = std::make_shared<Lexer::StringLexerInput>(R"(
def a(b, c, d, e = 5, w:1)
end
)");
    auto fileLexerInput = std::make_shared<Lexer::FileLexerInput>(
            "/home/user/.local/share/wineprefixes/lona/drive_c/Games/LonaRPG/UltraRevEngine/Main.rb");
    auto lexer = std::make_shared<Lexer::Lexer>(stringLexerInput);
    auto parser = std::make_shared<Parser::Parser>(lexer->getQueue());
    auto block = parser->parseProgram();
    IR::CodeGenerator codeGenerator;
    auto topLevel = new AST::FunctionDef("top_required", std::vector<AST::FuncDefArg *>(), nullptr, block);
    codeGenerator.codegenProgram(topLevel);
    codeGenerator.debugPrintModuleIR();

    auto &x = Lang::Symbol::getAllSymbols();

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
