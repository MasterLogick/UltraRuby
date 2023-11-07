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

Lang::Object *Uputs(Lang::Object *self, Lang::Object *arg) {
    std::cout << arg << std::endl;
    return Lang::PrimaryConstants::nilConst;
}

int main() {
    Lang::BasicClasses::init();
    Lang::PrimaryConstants::init();

    auto stringLexerInput = std::make_shared<Lexer::StringLexerInput>(R"(
def a(b, c, d, e = 5, w:1)
end

def b(expt)
  if expt
    puts 1
  end
end

b false
puts 123
puts 456
b true
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

    auto *main = new Lang::Object(Lang::BasicClasses::ObjectClass);
    Lang::FunctionDefMeta meta{1, 0, -1, 0, false, false, reinterpret_cast<void *>(&Uputs)};
    main->defineInstanceMethod(Lang::Symbol::get("puts"), &meta);

    std::cout <<"entering ruby code. self: "<< main << std::endl;
    auto resp = func(main);
    std::cout <<"executed ruby code. ret val class: "<< resp->getObjectClass()->getName() << std::endl;
    return 0;
}
