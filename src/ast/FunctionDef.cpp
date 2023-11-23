#include "FunctionDef.h"
#include "../lang/Object.h"

namespace UltraRuby {
namespace AST {

bool FunctionDef::hasNamedArgs() {
    for (const auto &item: args) {
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_MAP) {
            return true;
        }
    }
    return false;
}

bool FunctionDef::hasBlockArg() {
    for (const auto &item: args) {
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_BLOCK) {
            return true;
        }
    }
    return false;
}

FuncDefArg *FunctionDef::getBlockArg() {
    for (const auto &item: args) {
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_BLOCK) {
            return item;
        }
    }
    return nullptr;
}

FunctionDef::FunctionDef(std::string name, std::vector<FuncDefArg *> args, Statement *singleton, Block *body)
        : Statement(STMT_FUNC_DEF), name(std::move(name)), args(std::move(args)), singleton(singleton),
          body(body) {}

int FunctionDef::getArgc() {
    int argc = 0;
    for (const auto &arg: args) {
        auto type = arg->getType();
        if (type == FuncDefArg::AST_ARG_TYPE_BLOCK || type == FuncDefArg::AST_ARG_TYPE_MAP ||
            type == FuncDefArg::AST_ARG_TYPE_VARIADIC || arg->getDefaultValue() != nullptr) {
            return -1;
        }
        argc++;
    }
    if (argc > Lang::Object::MaxDirectArgsLen) {
        return -1;
    }
    return argc;
}
}
}