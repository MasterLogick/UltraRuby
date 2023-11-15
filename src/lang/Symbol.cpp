#include "Symbol.h"
#include "Heap.h"
#include "BasicClasses.h"
#include <map>
#include <mutex>
#include <iostream>

namespace UltraRuby {
namespace Lang {
std::map<std::string, Symbol *> symbols;
std::mutex m;

Symbol *Symbol::get(const std::string &str) {
    m.lock();
    auto f = symbols.find(str);
    if (f != symbols.end()) {
        m.unlock();
        return f->second;
    } else {
        auto *retVal = symbols.emplace(str, new Symbol(str)).first->second;
        m.unlock();
        return retVal;
    }
}

Symbol *Symbol::get(const char *str) {
    return get(std::string(str));
}

const std::map<std::string, Symbol *> &Symbol::getAllSymbols() {
    return symbols;
}
} // UltraRuby
} // Lang