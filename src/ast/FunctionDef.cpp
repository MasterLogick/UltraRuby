#include "FunctionDef.h"
#include "../lang/FunctionDefMeta.h"

namespace UltraRuby {
namespace AST {

int FunctionDef::getMaxArgsCount() {
    int count = 0;
    bool hasMap = false;
    for (const auto &item: args) {
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_NORMAL) {
            count++;
        }
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_MAP) {
            hasMap = true;
        }
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_BLOCK) {
            count++;
        }
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_VARIADIC) {
            count++;
        }
    }
    if (hasMap) {
        count++;
    }
    return count;
}

int FunctionDef::getMinArgsCount() {
    int count = 0;
    bool hasMap = false;
    for (const auto &item: args) {
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_NORMAL && item->getDefaultValue() == nullptr) {
            count++;
        }
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_MAP) {
            hasMap = true;
        }
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_BLOCK) {
            count++;
        }
        if (item->getType() == FuncDefArg::AST_ARG_TYPE_VARIADIC) {
            count++;
        }
    }
    if (hasMap) {
        count++;
    }
    return count;
}

bool FunctionDef::hasMapTypeArgs() {
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

Lang::FunctionDefMeta FunctionDef::createMethodDefMeta() {
    char requiredPrefix = 0;
    char requiredSuffix = 0;
    char optional = 0;
    char variadicPos = -1;
    bool namedArgs = false;
    bool capturesBlock = false;
    int i = 0;
    for (const auto &item: args) {
        switch (item->getType()) {
            case FuncDefArg::AST_ARG_TYPE_NORMAL: {
                if (item->getDefaultValue() != nullptr) {
                    optional++;
                } else if (optional != 0 || variadicPos != -1) {
                    requiredSuffix++;
                } else {
                    requiredPrefix++;
                }
                break;
            }
            case FuncDefArg::AST_ARG_TYPE_VARIADIC: {
                variadicPos = i;
                break;
            }
            case FuncDefArg::AST_ARG_TYPE_BLOCK: {
                capturesBlock = true;
                break;
            }
            case FuncDefArg::AST_ARG_TYPE_MAP: {
                // map args passed separately in one arg
                i--;
                namedArgs = true;
                break;
            }
        }
        i++;
    }
    return {requiredPrefix, optional, variadicPos, requiredSuffix, namedArgs, capturesBlock, nullptr};
}
}
}