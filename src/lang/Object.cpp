#include <iostream>
#include "Object.h"

namespace UltraRuby {
namespace Lang {
Object *Object::call0(Symbol *name) {
    return nullptr;
}

Object *Object::call1(Symbol *name, Object *arg1) {
    return nullptr;
}

Symbol *Object::defineInstanceMethod(Symbol *nameSymbol, int minArgs, int maxArgs, void *method) {
    std::cout << "called " << this << "->defineInstanceMethod(" << nameSymbol << ", " << minArgs << ", " << maxArgs
              << ", " << method << ")" << std::endl;
    return nameSymbol;
}
} // UltraRuby
} // Lang