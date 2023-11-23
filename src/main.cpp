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
#include "parser/ParseException.h"
#include <dlfcn.h>

using namespace UltraRuby;

Lang::Object *Uputs(Lang::Object *self, Lang::Object *arg) {
    std::cout << self << " " << arg << std::endl;
    return Lang::PrimaryConstants::NilConst;
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
class T
  def []=(a,b)
    puts "#{self}[#{a}]=#{b}"
  end

  def [](a)
    puts "#{self}[#{a},#{c}]"
    self
  end

  def +(b)
    puts "#{self} + #{b}"
  end

  def b
    puts "#{self}.b"
    "123"
  end

  def a=(b)
    puts "#{self}.a=#{b}"
  end

  def a
    puts "#{self}.a"
    "123"
  end
end

T.new[T.new]+=T.new.b
T.new.a*=5

)");
    auto lexer = std::make_shared<Lexer::Lexer>(stringLexerInput);
    auto parser = std::make_shared<Parser::Parser>(lexer->getQueue());
    AST::Block *block;
    try {
        block = parser->parseProgram();
    } catch (Parser::ParseException &e) {
        std::cout << "parsing exception: " << e.what() << std::endl;
        return -1;
    }
    IR::CodeGenerator codeGenerator;
    auto topLevel = new AST::FunctionDef("top_required", std::vector<AST::FuncDefArg *>(), nullptr, block);
    codeGenerator.codegenProgram(topLevel);
    codeGenerator.debugPrintModuleIR();

    Loader::EmittedObject eObj(codeGenerator);
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
    auto *main = Lang::PrimaryConstants::GlobalScope;
    main->defineInstanceMethod(Lang::Symbol::get("puts"), reinterpret_cast<void *>(&Uputs), 1, false, false);
    main->defineInstanceMethod(Lang::Symbol::get("raise"), reinterpret_cast<void *>(&Uraise), 1, false, false);
    std::cout << "entering ruby code. self: " << main << std::endl;
    try {
        auto resp = func(main);
        std::cout << "executed ruby code. ret val ptr: " << resp << std::endl;
    } catch (Lang::Exception &e) {
        std::cout << "uncaught lang exception: " << e.getException() << std::endl;
    }
    return 0;
}
