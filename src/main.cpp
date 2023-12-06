#include <iostream>
#include "lexer/Lexer.h"
#include "lexer/StringLexerInput.h"
#include "parser/Parser.h"
#include "ast/AST.h"
#include "lang/BasicClasses.h"
#include "lang/PrimaryConstants.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include "lang/Symbol.h"
#include "loader/EmittedObject.h"
#include "lang/Exception.h"
#include "parser/ParseException.h"
#include "lang/impl/NativeImplLoader.h"
#include "lang/String.h"
#include "ir/CodeModule.h"
#include "ir/FunctionGenerator.h"
#include <dlfcn.h>
#include <fstream>

using namespace UltraRuby;

Lang::Object *Uputs(Lang::Object *self, Lang::Object *arg) {
    if (arg) {
        if (arg->getObjectClass() == Lang::BasicClasses::StringClass) {
            std::cout << reinterpret_cast<Lang::String *>(arg)->getVal() << std::endl;
        } else {
            std::cout << self << " " << arg << std::endl;
        }
    } else {
        std::cout << self << " nil" << std::endl;
    }
    return Lang::PrimaryConstants::NilConst;
}

Lang::Object *Uraise(Lang::Object *self, Lang::Object *arg) {
    throw Lang::Exception(arg);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " filename" << std::endl;
        return -1;
    }
    Lang::BasicClasses::init();
    Lang::PrimaryConstants::init();
    Lang::Impl::NativeImplLoader::loadImpl();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "could not read file" << std::endl;
        return -1;
    }
    auto stringLexerInput = std::make_shared<Lexer::StringLexerInput>(std::string(buffer.data(), buffer.size()));
    auto lexer = std::make_shared<Lexer::Lexer>(stringLexerInput);
    auto parser = std::make_shared<Parser::Parser>(lexer->getQueue());
    AST::Block *block;
    try {
        block = parser->parseProgram();
    } catch (Lexer::SourceCodeException &e) {
        std::cout << "parsing exception: " << std::endl
                  << argv[1] << e.what() << std::endl;
        return -1;
    }
    IR::CodeModule codeModule;
    auto *topLevel = new AST::FunctionDef("top_required", block, 1, 1);

    IR::FunctionGenerator fg(&codeModule, topLevel);
    try {
        fg.emmitIR();
    } catch (Lexer::SourceCodeException &e) {
        std::cout << "parsing exception: " << std::endl
                  << argv[1] << e.what() << std::endl;
        return -1;
    }
    fg.getFunc()->setLinkage(llvm::GlobalValue::ExternalLinkage);

    codeModule.debugPrintModuleIR();

    Loader::EmittedObject eObj(codeModule);
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
