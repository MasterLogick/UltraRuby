#ifndef ULTRA_RUBY_LANG_SYMBOL_H
#define ULTRA_RUBY_LANG_SYMBOL_H

#include <string>
#include <map>
#include "Object.h"
#include "BasicClasses.h"

namespace UltraRuby {
namespace Lang {

class Symbol : public Object {
public:
    Symbol(const std::string &val) : Object(BasicClasses::SymbolClass), sym(val) {}

    static Symbol *get(const std::string &str);

    static Symbol *get(const char *str);

    static const std::map<std::string, Symbol *>& getAllSymbols();
private:
    std::string sym;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_SYMBOL_H
