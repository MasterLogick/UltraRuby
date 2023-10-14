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
#include "loader/EmittedObject.h"
#include "loader/ObjectArea.h"
#include <llvm/Object/ObjectFile.h>

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
def b(expt,aaa,gccc)
if expt
puts 1
end
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

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    Loader::EmittedObject eObj(codeGenerator);

    auto objExp = eObj.createELFObject();
    if (!objExp) {
        std::cout << "obj read error:" << toString(objExp.takeError()) << std::endl;
        return 1;
    }
    auto &ref = objExp.get();
    Loader::ObjectArea area;
    auto func = area.loadObject(ref);
    auto resp = func(nullptr);
    return 0;
}
