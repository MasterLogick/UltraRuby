#include "FunctionDef.h"
#include "../lang/Object.h"

namespace UltraRuby {
namespace AST {
int FunctionDef::getArgc() {
    if (!optionalArgs.empty() || !variadicArg.empty() || !namedArgs.empty() || !blockArg.empty()) {
        return -1;
    }
    int argc = requiredArgsPrefix.size();
    if (argc > Lang::Object::MaxDirectArgsLen) {
        return -1;
    }
    return argc;
}
}
}