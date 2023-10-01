#ifndef ULTRA_RUBY_LANG_SYMBOL_H
#define ULTRA_RUBY_LANG_SYMBOL_H

#include <string>
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Symbol : public Object {
public:
    static Symbol *get(std::string str);

    static Symbol *get(const char *str);

private:
    std::string sym;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_SYMBOL_H
