#include <iostream>
#include <llvm/IR/PassManager.h>
#include "lexer/Lexer.h"
#include "lexer/input/StringLexerInput.h"
#include "parser/Parser.h"
#include "ir/CodeGenerator.h"
#include "lexer/input/FileLexerInput.h"

using namespace UltraRuby;

int main() {
    auto fileLexerInput = std::make_shared<Lexer::FileLexerInput>(
            "/home/user/.local/share/wineprefixes/lona/drive_c/Games/LonaRPG/UltraRevEngine/Main.rb");
    auto lexer = std::make_shared<Lexer::Lexer>(fileLexerInput);
    auto parser = std::make_shared<Parser::Parser>(lexer->getQueue());
    auto block = parser->parseProgram();

//    auto stringLexerInput = std::make_shared<Lexer::StringLexerInput>("def f(type) 78*6+(1+type)");
//    IR::CodeGenerator codeGenerator;
//    Parser::Parser parser(lexer);
//    auto x = parser.parseFunctionDef();
//    auto *exp = codeGenerator.codegenFunctionDef(x);
//    exp->print(llvm::outs());
    return 0;
}
