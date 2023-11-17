#include <iostream>
#include "lexer/Lexer.h"
#include "lexer/input/StringLexerInput.h"
#include "parser/Parser.h"
#include "ir/CodeGenerator.h"
#include "ast/AST.h"
#include "lang/BasicClasses.h"
#include "lang/PrimaryConstants.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include "lang/Symbol.h"
#include "loader/EmittedObject.h"
#include "lang/Exception.h"
#include <dlfcn.h>

using namespace UltraRuby;

Lang::Object *Uputs(Lang::Object *self, Lang::Object *arg) {
    std::cout << self << " " << arg << std::endl;
    return &Lang::PrimaryConstants::NilConst;
}

Lang::Object *Uraise(Lang::Object *self, Lang::Object *arg) {
    throw Lang::Exception(arg);
}

int main() {
    Lang::BasicClasses::init();
    Lang::PrimaryConstants::init();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    auto stringLexerInput = std::make_shared<Lexer::StringLexerInput>(R"(
# frozen_string_literal: true
class C
  def a=(b)
    a
  end

  def a
    3
  end
end

)");
    auto lexer = std::make_shared<Lexer::Lexer>(stringLexerInput);
    auto parser = std::make_shared<Parser::Parser>(lexer->getQueue());
    auto block = parser->parseProgram();
    if (parser->hasErrors()) {
        return -1;
    }
    IR::CodeGenerator codeGenerator;
    auto topLevel = new AST::FunctionDef("top_required", std::vector<AST::FuncDefArg *>(), nullptr, block);
    codeGenerator.codegenProgram(topLevel);

    Loader::EmittedObject eObj(codeGenerator);
    codeGenerator.debugPrintModuleIR();
    char *cwd = getcwd(nullptr, 0);
    std::string path(cwd);
    free(cwd);
    path += "/";
    path += eObj.name;
    auto *f = dlopen(path.c_str(), RTLD_NOW);
    if (f == nullptr) {
        std::cout << dlerror() << std::endl;
        return -1;
    }
    auto *init = reinterpret_cast<void (*)()>(dlsym(f, "__init__"));
    if (init) {
        init();
    }
    auto *func = reinterpret_cast<Lang::Object *(*)(Lang::Object *)>(dlsym(f, "top_required"));
    if (func == nullptr) {
        std::cout << "not found entry point in library" << std::endl;
        return -1;
    }
    auto *main = new Lang::Object(Lang::BasicClasses::ObjectClass);
    Lang::FunctionDefMeta putsMeta{1, 0, false, 0, false, false, reinterpret_cast<void *>(&Uputs)};
    main->defineInstanceMethod(Lang::Symbol::get("puts"), &putsMeta);
    Lang::FunctionDefMeta raiseMeta{1, 0, false, 0, false, false, reinterpret_cast<void *>(&Uraise)};
    main->defineInstanceMethod(Lang::Symbol::get("raise"), &raiseMeta);
    std::cout << "entering ruby code. self: " << main << std::endl;
    try {
        auto resp = func(main);
        std::cout << "executed ruby code. ret val ptr: " << resp << std::endl;
    } catch (Lang::Exception &e) {
        std::cout << "uncaught lang exception" << std::endl;
    }
    return 0;
}
